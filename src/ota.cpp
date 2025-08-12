
#include "ota.h"

// Последний статус Wi-Fi
wl_status_t status = WL_IDLE_STATUS;

// Отслеживание обновления
bool begin = false;

// Порт получения данных
int port = 0;

// Размер обновления
int size = 0;

// Протокол UDP
WiFiUDP Udp;

void otaSetup() {
    status = WL_IDLE_STATUS;
    if (!begin) {
        xTaskCreatePinnedToCore(otaBegin, "ota", 8192, NULL, 1, NULL, 1);
        begin = true;
    }
}

void otaBegin(void *params) {
    while (true) {
        otaLoop();
    }
}

void otaLoop() {
    // Изменение статуса Wi-Fi
    if (status != WiFi.status()) {
        status = WiFi.status();
        if (status == WL_CONNECTED && port > 0) {
            Udp.stop();
#if OTA_DEBUG
            debug("V: Udp.begin(%d)\n", port);
#endif
            Udp.begin(port);
            if (size > 0) {
                otaNotify(WiFi.localIP().toString().c_str());
                otaMain();
            }
        } else if (status == WL_CONNECT_FAILED || status == WL_CONNECTION_LOST || status == WL_DISCONNECTED) {
#if OTA_DEBUG
            debug("E: WiFi.status: %d\n", status);
#endif
            otaError("wifi");
        }
    }
    delay(100);
}

void otaMain() {
    int downloadSize = 0;
    bool updateBegin = false;
    int updateSize = size;
    size = 0;
    esp_err_t r = 0;
    esp_ota_handle_t updateHandle = 0;
    const esp_partition_t *updatePartition = esp_ota_get_next_update_partition(NULL);
    if (updatePartition == nullptr) {
        return otaError("partition");
    }
    while (true) {
        char packet[4096];
        int offset = 0;
        int length = 0;
        for (int i = 0; i < 4; i ++) {
            length = 0;
            while (length <= 0) {
                length = Udp.parsePacket();
            }
            length = Udp.read(packet + offset, length > 1024 ? 1024 : length);
            downloadSize += length;
            offset += length;
            if (Udp.remoteIP() != IPADDR_ANY && Udp.remoteIP() != WiFi.localIP() && Udp.remotePort() > 0) {
                Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                Udp.printf("OTA:RX:%d", downloadSize);
                Udp.endPacket();
            }
            /*
            char message[16];
            snprintf(message, 15, "RX:%d", downloadSize);
            otaNotify(message);
            */
            if (downloadSize == updateSize) {
                break;
            }
            if (downloadSize > updateSize) {
                return otaError("download");
            }
        }
        if (!updateBegin) {
            updateBegin = true;
            r = esp_ota_begin(updatePartition, OTA_WITH_SEQUENTIAL_WRITES, &updateHandle);
            if (r != ESP_OK) {
                return otaError("begin");
            }
        }
        r = esp_ota_write(updateHandle, packet, offset);
        if (r != ESP_OK) {
            esp_ota_abort(updateHandle);
            return otaError("write");
        }
        if (downloadSize == updateSize) {
            r = esp_ota_end(updateHandle);
            if (r != ESP_OK) {
                esp_ota_abort(updateHandle);
                return otaError("end");
            }
            r = esp_ota_set_boot_partition(updatePartition);
            if (r != ESP_OK) {
                return otaError("boot");
            }
            return esp_restart();
        }
    }
}

bool otaHandle(const char *packet) {
    size_t length = strlen(packet);
    if (strncmp(packet, "AT+CWJAP=", 9) == 0) {
        // Подключение к Wi-Fi: AT+CWJAP=SSID,password
        char ssid[32] = {0};
        char password[64] = {0};
        // const char *ssid = packet + 9;
        // const char *password = packet + length;
        for (int i = 9; i < length; i++) {
            if (packet[i] == ',') {
                strncat(ssid, packet + 9, min(i - 9, 31));
                strncat(password, packet + i + 1, 63);
                break;
            }
        }
        status = WL_IDLE_STATUS;
#if OTA_DEBUG
        debug("V: WiFi.begin(\"%s\", \"%s\")\n", ssid, password);
#endif
        WiFi.begin(ssid, password);
        return true;
    }
    if (strncmp(packet, "AT+CIPSERVER=1,", 15) == 0) {
        // Открытие порта для приёма данных: AT+CIPSERVER=1,port
        port = atoi(packet + 15);
        return true;
    }
    if (strncmp(packet, "AT+CIPSEND=", 11) == 0) {
        // Получение размера обновления
        size = atoi(packet + 11);
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
