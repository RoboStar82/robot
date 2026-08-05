
#pragma once

#include <EspUsbHost.h>

#include "config.h"

class USBHost {
   public:
    USBHost();
    ~USBHost();

    void begin();

    void end();

    void onDeviceConnected(const EspUsbHostDeviceInfo& device);
    void onDeviceDisconnected(const EspUsbHostDeviceInfo& device);

    void onGamepad(const EspUsbHostGamepadEvent& event);
    void onHIDInput(const EspUsbHostHIDInput& input);

   protected:
    EspUsbHost espUsbHost;
};

extern USBHost usb;
