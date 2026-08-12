
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_USB

#include "usb.h"

#ifdef ROBOT_HAS_CONTROLLER
#include "controller.h"
#endif

#include "delay.h"
#include "print.h"

USB usb;

USB::USB() {}

USB::~USB() {}

void USB::begin() {
    print("[USB] begin\n");
    if (!taskUsbLibStarted) {
        xTaskCreate(taskUsbLib, "usb_lib_task", 4096, xTaskGetCurrentTaskHandle(), 1, &taskUsbLibStarted);
        ulTaskNotifyTakeMS(false, 1000);
    }
    if (!taskUsbDevStarted) {
        xTaskCreate(taskUsbDev, "usb_dev_task", 4096, NULL, 1, &taskUsbDevStarted);
    }
    if (!taskUsbHidStarted) {
        xTaskCreate(taskUsbHid, "usb_hid_task", 4096, NULL, 1, &taskUsbHidStarted);
    }
}

void USB::end() {
    print("[USB] end\n");
    if (taskUsbDevStarted) {
        vTaskDelete(taskUsbDevStarted);
        taskUsbDevStarted = nullptr;
    }
    if (taskUsbHidStarted) {
        vTaskDelete(taskUsbHidStarted);
        taskUsbHidStarted = nullptr;
        hid_host_uninstall();
    }
    if (taskUsbLibStarted) {
        usb_host_uninstall();
    }
}

void USB::taskUsbLib(void* arg) {
    usb_host_config_t hostConfig = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
        .enum_filter_cb = onEnumFilter,
    };
    usb_host_install(&hostConfig);
    xTaskNotifyGive((TaskHandle_t)arg);
    while (true) {
        uint32_t eventFlags;
        usb_host_lib_handle_events(100, &eventFlags);
        if (eventFlags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
        vTaskDelayMS(1);
    }
    usb_host_uninstall();
    vTaskDelete(NULL);
}

bool USB::onEnumFilter(const usb_device_desc_t* descriptor, uint8_t* configuration) {
    print("[USB] enumeration: %04x:%04x\n", descriptor->idVendor, descriptor->idProduct);
    return true;
}

void USB::taskUsbDev(void* arg) {
    usb_host_client_config_t clientConfig = {
        .is_synchronous = false,
        .max_num_event_msg = 4,
        .async = {
            .client_event_callback = onClientEvent,
            .callback_arg = NULL,
        },
    };
    usb_host_client_handle_t client;
    usb_host_client_register(&clientConfig, &client);
    while (true) {
        usb_host_client_handle_events(client, 100);
        vTaskDelayMS(1);
    }
    usb_host_client_deregister(client);
}

void USB::onClientEvent(const usb_host_client_event_msg_t* event, void* arg) {
    switch (event->event) {
        case USB_HOST_CLIENT_EVENT_NEW_DEV:
            print("[USB] device begin %u\n", event->new_dev.address);
            break;
        case USB_HOST_CLIENT_EVENT_DEV_GONE:
            print("[USB] device end\n");
            break;
    }
}

void USB::taskUsbHid(void* arg) {
    hid_host_driver_config_t driver_config = {
        .create_background_task = false,
        .callback = onHidDriverEvent,
        .callback_arg = NULL,
    };
    hid_host_install(&driver_config);
    while (true) {
        hid_host_handle_events(100);
        vTaskDelayMS(1);
    }
    hid_host_uninstall();
}

void USB::onHidDriverEvent(hid_host_device_handle_t device, hid_host_driver_event_t event, void* arg) {
    switch (event) {
        case HID_HOST_DRIVER_EVENT_CONNECTED:
            print("[USB] device HID begin\n");
            hid_host_device_config_t deviceConfig = {
                .callback = onHidDeviceEvent,
                .callback_arg = NULL,
            };
            hid_host_device_open(device, &deviceConfig);
            hid_host_device_start(device);
            break;
    }
}

void USB::onHidDeviceEvent(hid_host_device_handle_t device, hid_host_interface_event_t event, void* arg) {
    switch (event) {
        case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
            onHidDeviceInput(device, arg);
            break;
        case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
            print("[USB] device HID end\n");
            hid_host_device_close(device);
            break;
        case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
            print("[USB] device HID error\n");
            break;
    }
}

void USB::onHidDeviceInput(hid_host_device_handle_t device, void* arg) {
    const usb_device_desc_t* descriptor;
    if (!getDeviceDescriptor(device, &descriptor)) {
        size_t length = 0;
        uint8_t data[16] = {0};
        if (!hid_host_device_get_raw_input_report_data(device, data, sizeof(data), &length)) {
#ifdef ROBOT_HAS_CONTROLLER
            controller.onInput(data, length, descriptor->idVendor, descriptor->idProduct);
#endif
        }
    }
}

esp_err_t USB::getDeviceDescriptor(hid_host_device_handle_t device, const usb_device_desc_t** descriptor) {
    hid_interface_t* iface = (hid_interface_t*)device;
    return usb_host_get_device_descriptor(iface->parent->dev_hdl, descriptor);
}

#endif
