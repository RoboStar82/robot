
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_http_server.h>
#include <lwip/sockets.h>

#include "config.h"
#include "print.h"

class OTAHttp {
   public:
    OTAHttp();
    ~OTAHttp();

    void begin();

    void end();

    esp_err_t onOpen(int fd);
    void onClose(int fd);

    esp_err_t onScriptPost(httpd_req_t* request);
    esp_err_t onWebSocketGet(httpd_req_t* request);
    esp_err_t onNotFound(httpd_req_t* request);

   protected:
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t configScript = {
        .uri = "/script",
        .method = HTTP_POST,
        .user_ctx = NULL,
    };

    httpd_uri_t configWebSocket = {
        .uri = "/ws",
        .method = HTTP_GET,
        .user_ctx = NULL,
        .is_websocket = true,
    };

    static inline esp_err_t onOpen(httpd_handle_t server, int fd);
    static inline void onClose(httpd_handle_t server, int fd);

    static inline esp_err_t onScript(httpd_req_t* request);
    static inline esp_err_t onWebSocket(httpd_req_t* request);

    static inline esp_err_t on404(httpd_req_t* request, httpd_err_code_t e);
};

extern OTAHttp otaHttp;
