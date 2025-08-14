
#include "ota.h"

// Идёт обновление
TaskHandle_t otaTask;
TaskHandle_t *otaTaskCreated = nullptr;

// Имя сети Wi-Fi
char ssid[32] = {0};
// Пароль Wi-Fi
char password[64] = {0};

// Порт получения данных
int port = 3232;

void otaSetup() {
    if (otaTaskCreated == nullptr && ssid[0] != 0 && password[0] != 0 && port > 0) {
        xTaskCreatePinnedToCore(otaBegin, "ota", 8192, NULL, 1, &otaTask, 1);
        otaTaskCreated = &otaTask;
    }
}

void otaBegin(void *params) {
    WiFiServer server(port);
    WiFiClient client;
    unsigned int updateSize = 0;
    unsigned int updateRead = 0;
    uint8_t updateInit[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    const esp_partition_t *updatePartition;
    esp_ota_handle_t updateHandle = 0;
    bool updateBegin = false;
    esp_err_t e;
#if OTA_DEBUG
    debug("V: OTA: WiFi.begin: \"%s\"; \"%s\"\n", ssid, password);
#endif
    WiFi.begin(ssid, password);
    for (int i = 0; i < 100; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
        delay(100);
    }
    if (WiFi.status() != WL_CONNECTED) {
#if OTA_DEBUG
        debug("E: OTA: WiFi.status: %d\n", WiFi.status());
#endif
        otaError("wifi");
        goto end;
    }
    server.begin();
    otaNotify(WiFi.localIP(), port);
    for (int i = 0; i < 100; i++) {
        if (client = server.available()) {
            break;
        }
        delay(100);
    }
    if (!client) {
#if OTA_DEBUG
        debug("E: OTA: WiFi.client\n");
#endif
        otaError("client");
        goto end;
    }
    for (int i = 0; i < 100; i++) {
        if (client.available() >= 8) {
            break;
        }
        delay(100);
    }
    if (client.available() < 8) {
#if OTA_DEBUG
        debug("E: OTA: WiFi.client.available: %d\n", client.available());
#endif
        otaError("init");
        goto end;
    }
    client.read(updateInit, 8);
    if (updateInit[0] != 'O' || updateInit[1] != 'T' || updateInit[2] != 'A' || updateInit[3] != ':') {
#if OTA_DEBUG
        debug("E: OTA: WiFi.client.read: %02x%02x%02x%02x\n", updateInit[0], updateInit[1], updateInit[2], updateInit[3]);
#endif
        otaError("magic");
        goto end;
    }
    updateSize = (updateInit[4] << 0) | (updateInit[5] << 8) | (updateInit[6] << 16) | (updateInit[7] << 24);
    if (!updateSize) {
#if OTA_DEBUG
        debug("E: OTA: WiFi.update.size: %d\n", updateSize);
#endif
        otaError("size");
        goto end;
    }
#if OTA_DEBUG
    debug("V: OTA: size: %d\n", updateSize);
#endif
    updatePartition = esp_ota_get_next_update_partition(NULL);
    if (updatePartition == nullptr) {
#if OTA_DEBUG
        debug("E: OTA: esp_ota_get_next_update_partition: NULL\n");
#endif
        otaError("partition");
        goto end;
    }
    e = esp_ota_begin(updatePartition, OTA_WITH_SEQUENTIAL_WRITES, &updateHandle);
    if (e != ESP_OK) {
#if OTA_DEBUG
        debug("E: OTA: esp_ota_begin: %d\n", e);
#endif
        otaError("begin");
        goto end;
    }
    updateBegin = true;
    while (updateRead < updateSize) {
        uint8_t packet[4096];
        int length = 0;
        while (true) {
            for (int i = 0; i < 100; i++) {
                if (client.available() > 0) {
                    break;
                }
                delay(100);
            }
            if (client.available() <= 0) {
#if OTA_DEBUG
                debug("E: OTA: WiFi.client.available: %d\n", client.available());
#endif
                otaError("data");
                goto end;
            }
            int size = client.read(packet + length, min(client.available(), 4096 - length));
            if (size <= 0) {
                continue;
            }
            length += size;
            updateRead += size;
#if OTA_DEBUG
            debug("V: OTA: read: %d\n", updateRead);
#endif
            if (length == 4096 || updateRead >= updateSize) {
                break;
            }
        }
        if (length > 0) {
            e = esp_ota_write(updateHandle, packet, length);
            if (e != ESP_OK) {
#if OTA_DEBUG
                debug("E: OTA: esp_ota_write: %d\n", e);
#endif
                otaError("write");
                goto end;
            }
        }
    }
    e = esp_ota_end(updateHandle);
    if (e != ESP_OK) {
#if OTA_DEBUG
        debug("E: OTA: esp_ota_end: %d\n", e);
#endif
        otaError("end");
        goto end;
    }
    e = esp_ota_set_boot_partition(updatePartition);
    if (e != ESP_OK) {
#if OTA_DEBUG
        debug("E: OTA: esp_ota_set_boot_partition: %d\n", e);
#endif
        otaError("reboot");
        goto end;
    }
#if OTA_DEBUG
    debug("V: OTA: restart\n");
#endif
    esp_restart();
end:
    if (updateBegin) {
        esp_ota_abort(updateHandle);
    }
    if (client) {
        client.stop();
    }
    if (server) {
        server.stop();
    }
    WiFi.disconnect();
    otaTaskCreated = nullptr;
    vTaskDelete(NULL);
}

bool otaHandle(const char *packet) {
    int length = strlen(packet);
    if (strncmp(packet, "AT+CWJAP=", 9) == 0) {
        // Подключение к Wi-Fi: AT+CWJAP="SSID","password"
        ssid[0] = 0;
        password[0] = 0;
        if (packet[9] == '"' && packet[length - 1] == '"') {
            for (int i = 10; i < length; i++) {
                if (packet[i] == '"' && packet[i + 1] == ',' && packet[i + 2] == '"') {
                    strncat(ssid, packet + 10, min(i - 10, 31));
                    strncat(password, packet + i + 3, min(length - i - 4, 63));
                }
            }
        } else {
            for (int i = 9; i < length; i++) {
                if (packet[i] == ',') {
                    strncat(ssid, packet + 9, min(i - 9, 31));
                    strncat(password, packet + i + 1, min(length - i - 1, 63));
                    break;
                }
            }
        }
        return true;
    }
    if (strncmp(packet, "AT+CIPSERVER=1,", 15) == 0) {
        // Открытие порта для приёма данных: AT+CIPSERVER=1,port
        port = atoi(packet + 15);
        return true;
    }
    if (strncmp(packet, "AT+CIUPDATE", 11) == 0) {
        // Начало обновления
        otaSetup();
        return true;
    }
    return false;
}

void otaFinish() {
    const esp_partition_t *runningPartition = esp_ota_get_running_partition();
    esp_ota_img_states_t otaState;
    if (esp_ota_get_state_partition(runningPartition, &otaState) == ESP_OK) {
        if (otaState == ESP_OTA_IMG_PENDING_VERIFY) {
            if (otaVerify()) {
#if OTA_DEBUG
                println("V: OTA: Success");
#endif
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
#if OTA_DEBUG
                println("E: OTA: Failure");
#endif
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }
    esp_app_desc_t otaDescription;
    esp_ota_get_partition_description(runningPartition, &otaDescription);
#if OTA_DEBUG
    debug("V: OTA: Firmware: %s; %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
          BUILD_DATETIME,
          otaDescription.app_elf_sha256[0],
          otaDescription.app_elf_sha256[1],
          otaDescription.app_elf_sha256[2],
          otaDescription.app_elf_sha256[3],
          otaDescription.app_elf_sha256[4],
          otaDescription.app_elf_sha256[5],
          otaDescription.app_elf_sha256[6],
          otaDescription.app_elf_sha256[7],
          otaDescription.app_elf_sha256[8],
          otaDescription.app_elf_sha256[9],
          otaDescription.app_elf_sha256[10],
          otaDescription.app_elf_sha256[11],
          otaDescription.app_elf_sha256[12],
          otaDescription.app_elf_sha256[13],
          otaDescription.app_elf_sha256[14],
          otaDescription.app_elf_sha256[15],
          otaDescription.app_elf_sha256[16],
          otaDescription.app_elf_sha256[17],
          otaDescription.app_elf_sha256[18],
          otaDescription.app_elf_sha256[19],
          otaDescription.app_elf_sha256[20],
          otaDescription.app_elf_sha256[21],
          otaDescription.app_elf_sha256[22],
          otaDescription.app_elf_sha256[23],
          otaDescription.app_elf_sha256[24],
          otaDescription.app_elf_sha256[25],
          otaDescription.app_elf_sha256[26],
          otaDescription.app_elf_sha256[27],
          otaDescription.app_elf_sha256[28],
          otaDescription.app_elf_sha256[29],
          otaDescription.app_elf_sha256[30],
          otaDescription.app_elf_sha256[31]);
#endif
}

bool otaVerify() {
    gpio_config_t io;
    io.intr_type = GPIO_INTR_DISABLE;
    io.mode = GPIO_MODE_INPUT;
    io.pin_bit_mask = 1ULL;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    bool r = gpio_get_level(GPIO_NUM_0);
    gpio_reset_pin(GPIO_NUM_0);
    return r;
}
