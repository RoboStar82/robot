
#pragma once

#include <Arduino.h>
#include <usb/usb_host.h>

#include "hid_host.h"

typedef struct {
    hid_host_device_handle_t device_handle;
    hid_host_driver_event_t driver_event;
    void* arg;
} usb_hid_host_device_event_t;

typedef struct {
    uint8_t b1;
    uint8_t lx;
    uint8_t ly;
    uint8_t rx;
    uint8_t ry;
    uint8_t dpad : 4;
    uint8_t x : 1;
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t y : 1;
    uint8_t lzu : 1;
    uint8_t rzu : 1;
    uint8_t lzd : 1;
    uint8_t rzd : 1;
    uint8_t back : 1;
    uint8_t start : 1;
    uint8_t lt : 1;
    uint8_t rt : 1;
    uint8_t mode : 8;
} __attribute__((packed)) usb_hid_controller_state_t;

class USB {
   public:
    void begin();

   protected:
    static void hostLibTask(void* arg);

    static QueueHandle_t hidHostQueue;
    static void hidHostTask(void* arg);

    static void hidHostDriverEventHandler(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg);
    static void hidHostDriverEventCallback(hid_host_device_handle_t device_handle, const hid_host_driver_event_t driver_event, void* arg);
    static void hidHostInterfaceEventCallback(hid_host_device_handle_t device_handle, const hid_host_interface_event_t interface_event, void* arg);
    static void hidHostInterfaceInputReportEventHandler(hid_host_device_handle_t device_handle);
};

extern USB usb;
