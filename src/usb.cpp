
#include "usb.h"

#include "controller.h"

USB usb;

USB::USB() {}

USB::~USB() {}

void USB::begin() {
    xTaskCreate(hidHostTask, "usb_hid_host_task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(hostLibTask, "usb_host_lib_task", 4096, xTaskGetCurrentTaskHandle(), 1, NULL, 0);
    ulTaskNotifyTake(false, 1000);
    const hid_host_driver_config_t driver_config = {
        .create_background_task = true,
        .task_priority = 1,
        .stack_size = 4096,
        .core_id = 0,
        .callback = hidHostDriverEventCallback,
        .callback_arg = NULL,
    };
    hid_host_install(&driver_config);
}

void USB::hostLibTask(void* arg) {
    const usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    usb_host_install(&host_config);
    xTaskNotifyGive((TaskHandle_t)arg);
    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
    }
    vTaskDelay(50);
    usb_host_uninstall();
    vTaskDelete(NULL);
}

QueueHandle_t USB::hidHostQueue = xQueueCreate(10, sizeof(usb_hid_host_device_event_t));

void USB::hidHostTask(void* arg) {
    usb_hid_host_device_event_t device_event;
    while (true) {
        if (xQueueReceive(hidHostQueue, &device_event, pdMS_TO_TICKS(50))) {
            hidHostDriverEventHandler(device_event.device_handle, device_event.driver_event, device_event.arg);
        }
    }
    xQueueReset(hidHostQueue);
    vTaskDelete(NULL);
}

void USB::hidHostDriverEventHandler(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg) {
    switch (driver_event) {
        case HID_HOST_DRIVER_EVENT_CONNECTED:
            log_i("USB HID: Connected");
            const hid_host_device_config_t device_config = {
                .callback = hidHostInterfaceEventCallback,
                .callback_arg = NULL,
            };
            hid_host_device_open(device_handle, &device_config);
            hid_host_device_start(device_handle);
            break;
    }
}

void USB::hidHostDriverEventCallback(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg) {
    const usb_hid_host_device_event_t device_event = {
        .device_handle = device_handle,
        .driver_event = driver_event,
        .arg = arg,
    };
    xQueueSend(hidHostQueue, &device_event, 0);
}

void USB::hidHostInterfaceEventCallback(hid_host_device_handle_t device_handle, const hid_host_interface_event_t interface_event, void* arg) {
    switch (interface_event) {
        case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
            hidHostInterfaceInputReportEventHandler(device_handle);
            break;
        case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
            log_i("USB HID: Disconnected");
            hid_host_device_close(device_handle);
            break;
        case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
            log_e("USB HID: Transfer error");
            break;
    }
}

void USB::hidHostInterfaceInputReportEventHandler(hid_host_device_handle_t device_handle) {
    const usb_device_desc_t* device_descriptor;
    hid_iface_t* hid_iface = (hid_iface_t*)device_handle;
    usb_host_get_device_descriptor(hid_iface->parent->dev_hdl, &device_descriptor);
    if (device_descriptor->idVendor == 0x046d) {
        if (device_descriptor->idProduct == 0xc216 || device_descriptor->idProduct == 0xc218 || device_descriptor->idProduct == 0xc219) {
            uint8_t data[8] = {0};
            size_t length = 0;
            hid_host_device_get_raw_input_report_data(device_handle, data, 8, &length);
            if (length == 8 && data[0] == 0x01) {
                usb_hid_controller_state_t* state = (usb_hid_controller_state_t*)data;
                controller_state_t controller_state = {
                    .lx = (int8_t)(state->lx < 0x70 ? -((0x7f - state->lx) >> 4) : (state->lx >= 0x90 ? (state->lx - 0x80) >> 4 : 0)),
                    .ly = (int8_t)(state->ly < 0x70 ? (0x7f - state->ly) >> 4 : (state->ly >= 0x90 ? -((state->ly - 0x80) >> 4) : 0)),
                    .rx = (int8_t)(state->rx < 0x70 ? -((0x7f - state->rx) >> 4) : (state->rx >= 0x90 ? (state->rx - 0x80) >> 4 : 0)),
                    .ry = (int8_t)(state->ry < 0x70 ? (0x7f - state->ry) >> 4 : (state->ry >= 0x90 ? -((state->ry - 0x80) >> 4) : 0)),
                    .dx = (int8_t)(state->dpad == 8 || state->dpad == 0 || state->dpad == 4 ? 0 : (state->dpad > 0 && state->dpad < 4 ? 1 : -1)),
                    .dy = (int8_t)(state->dpad == 8 || state->dpad == 2 || state->dpad == 6 ? 0 : (state->dpad < 2 || state->dpad > 6 ? 1 : -1)),
                    .lz = (int8_t)(state->lzd ? (state->lzu ? -2 : -1) : (state->lzu ? 1 : 0)),
                    .rz = (int8_t)(state->rzd ? (state->rzu ? -2 : -1) : (state->rzu ? 1 : 0)),
                    .lt = state->lt ? true : false,
                    .rt = state->rt ? true : false,
                    .a = state->a ? true : false,
                    .b = state->b ? true : false,
                    .x = state->x ? true : false,
                    .y = state->y ? true : false,
                    .start = state->start ? true : false,
                    .back = state->back ? true : false,
                    .mode = state->mode,
                };
                controller.setState(controller_state);
            }
        }
    }
}
