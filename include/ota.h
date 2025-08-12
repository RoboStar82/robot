
#pragma once

#ifndef OTA_DEBUG
#define OTA_DEBUG 1
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_ota_ops.h>

#include "print.h"
#include "version.h"

void otaSetup();

void otaBegin(void *params);

void otaLoop();

void otaMain();

bool otaHandle(const char *packet);

void otaNotify(const char *message);

void otaFinish();

bool otaVerify();

void otaError(const char *error);
