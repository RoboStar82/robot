
#pragma once

#include <Arduino.h>
#include <PsychicHttpServer.h>
#include <PsychicWebSocket.h>

#include "config.h"
#include "print.h"

class OTAHttp {
   public:
    OTAHttp();
    ~OTAHttp();

    void begin();

    void end();

   protected:
    PsychicHttpServer server;
    PsychicWebSocketHandler websocket;

    void onOpen(PsychicClient* client);
    void onClose(PsychicClient* client);

    esp_err_t onScriptPost(PsychicRequest* request, PsychicResponse* response);

    void onWsOpen(PsychicWebSocketClient* client);
    void onWsClose(PsychicWebSocketClient* client);
    esp_err_t onWsFrame(PsychicWebSocketRequest* request, httpd_ws_frame* frame);

    esp_err_t onNotFound(PsychicRequest* request, PsychicResponse* response);
};

extern OTAHttp otaHttp;
