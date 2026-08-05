
#include "config.h"

#ifdef ROBOT_HAS_USB

#include "print.h"
#include "usb.h"

USBHost usb;

USBHost::USBHost() {
    espUsbHost.onDeviceConnected([this](const EspUsbHostDeviceInfo& device) {
        return this->onDeviceConnected(device);
    });
    espUsbHost.onDeviceDisconnected([this](const EspUsbHostDeviceInfo& device) {
        return this->onDeviceDisconnected(device);
    });
    espUsbHost.onGamepad([this](const EspUsbHostGamepadEvent& event) {
        return this->onGamepad(event);
    });
    espUsbHost.onHIDInput([this](const EspUsbHostHIDInput& input) {
        return this->onHIDInput(input);
    });
}

USBHost::~USBHost() {}

void USBHost::begin() {
    if (!espUsbHost.begin()) {
        print("[USB] %s\n", espUsbHost.lastErrorName());
    }
}

void USBHost::end() {
    espUsbHost.end();
}

void USBHost::onDeviceConnected(const EspUsbHostDeviceInfo& device) {
    print("[USB] connected\n");
    espUsbHostPrint(device, RobotSerial);
}

void USBHost::onDeviceDisconnected(const EspUsbHostDeviceInfo& device) {
    print("[USB] disconnected\n");
}

void USBHost::onGamepad(const EspUsbHostGamepadEvent& event) {
    print("[USB] gamepad\n");
    RobotSerial.printf("[USB] gamepad %d\n", event.reportLength);
    RobotSerial.printf("[USB] 0x");
    for (int i = 0; i < event.reportLength; i++) {
        if (event.reportData[i] < 0x10) {
            RobotSerial.print('0');
        }
        RobotSerial.print(event.reportData[i], HEX);
    }
    RobotSerial.println();
}

void USBHost::onHIDInput(const EspUsbHostHIDInput& input) {
    print("[USB] HID input\n");
}

#endif
