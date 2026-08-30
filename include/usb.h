
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <usb/hid_host.h>
#ifdef __cplusplus
}
#endif

#include "config.h"

typedef struct {
    void* tailq_entry;
    SemaphoreHandle_t device_busy;
    SemaphoreHandle_t ctrl_xfer_done;
    usb_transfer_t* ctrl_xfer;
    usb_device_handle_t dev_hdl;
    uint8_t dev_addr;
} hid_device_t;

typedef struct {
    void* tailq_entry;
    hid_device_t* parent;
    hid_host_dev_params_t dev_params;
    uint8_t ep_in;
    uint16_t ep_in_mps;
    uint8_t country_code;
    uint16_t report_desc_size;
    uint8_t* report_desc;
    usb_transfer_t* in_xfer;
    hid_host_interface_event_cb_t user_cb;
    void* user_cb_arg;
    int state;
    int last_state;
} hid_interface_t;

class USB {
   public:
    USB();
    virtual ~USB() = default;

    void begin();

    void end();

   protected:
    static void taskUsbLib(void* arg);
    static void taskUsbDev(void* arg);
    static void taskUsbHid(void* arg);

    static bool onEnumFilter(const usb_device_desc_t* device, uint8_t* configuration);

    static void onClientEvent(const usb_host_client_event_msg_t* event, void* arg);

    static void onHidDriverEvent(hid_host_device_handle_t device, const hid_host_driver_event_t event, void* arg);

    static void onHidDeviceEvent(hid_host_device_handle_t device, const hid_host_interface_event_t event, void* arg);
    static void onHidDeviceInput(hid_host_device_handle_t device, void* arg);

    static esp_err_t getDeviceDescriptor(hid_host_device_handle_t device, const usb_device_desc_t** descriptor);

    TaskHandle_t taskUsbLibHandle = nullptr;
    TaskHandle_t taskUsbDevHandle = nullptr;
    TaskHandle_t taskUsbHidHandle = nullptr;
};

extern USB usb;
