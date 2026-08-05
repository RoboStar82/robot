
#pragma once

/*
#include <EspUsbHost.h>
*/

#ifdef __cplusplus
extern "C" {
#endif
#include <usb/hid_host.h>
#ifdef __cplusplus
}
#endif

#include "config.h"

class USB {
   public:
    USB();
    ~USB();

    void begin();

    void end();

    static void taskUsbLib(void* arg);
    static void taskUsbDev(void* arg);
    static void taskUsbHid(void* arg);

    static bool onEnumFilter(const usb_device_desc_t* device, uint8_t* configuration);

    static void onClientEvent(const usb_host_client_event_msg_t* event, void* arg);

    static void onHidDriverEvent(hid_host_device_handle_t device, const hid_host_driver_event_t event, void* arg);

    static void onHidDeviceEvent(hid_host_device_handle_t device, const hid_host_interface_event_t event, void* arg);

    TaskHandle_t taskUsbLibStarted = nullptr;
    TaskHandle_t taskUsbDevStarted = nullptr;
    TaskHandle_t taskUsbHidStarted = nullptr;

   protected:
};

extern USB usb;
