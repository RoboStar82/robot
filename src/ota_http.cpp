
#include "config.h"

#ifdef ROBOT_HAS_OTA_HTTP

#include "ota_http.h"

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

OTAHttp otaHttp;

OTAHttp::OTAHttp() {
    server.onOpen([this](PsychicClient* client) {
        this->onOpen(client);
    });
    server.onClose([this](PsychicClient* client) {
        this->onClose(client);
    });
    server.on("/script", HTTP_POST, [this](PsychicRequest* request, PsychicResponse* response) {
        return this->onScriptPost(request, response);
    });
    websocket.onOpen([this](PsychicWebSocketClient* client) {
        this->onWsOpen(client);
    });
    websocket.onClose([this](PsychicWebSocketClient* client) {
        this->onWsClose(client);
    });
    websocket.onFrame([this](PsychicWebSocketRequest* request, httpd_ws_frame_t* frame) {
        return this->onWsFrame(request, frame);
    });
    server.on("/ws", &websocket);
    server.onNotFound([this](PsychicRequest* request, PsychicResponse* response) {
        return this->onNotFound(request, response);
    });
}

OTAHttp::~OTAHttp() {}

void OTAHttp::begin() {
    server.begin();
}

void OTAHttp::end() {
    server.end();
}

void OTAHttp::onOpen(PsychicClient* client) {
    print("[HTTP] begin %s\n", client->remoteIP().toString().c_str());
}

void OTAHttp::onClose(PsychicClient* client) {
    print("[HTTP] end %s\n", client->remoteIP().toString().c_str());
}

esp_err_t OTAHttp::onScriptPost(PsychicRequest* request, PsychicResponse* response) {
    response->setContentType("text/plain");
#ifdef ROBOT_HAS_SCRIPT
    if (request->contentLength() > 0) {
        std::string content;
        bool finish = false;
        script.run({
            .filename = "<OTA HTTP>",
            .source = request->bodyCStr(),
            .length = request->contentLength(),
            .write = [&](const uint8_t* buffer, size_t length) {
                if (buffer && !finish) {
                    content.append(reinterpret_cast<const char*>(buffer), length);
                } else {
                    finish = true;
                }
            },
        });
        while (!finish) {
            vTaskDelay(1);
        }
        response->setContent((const uint8_t*)content.c_str(), content.length());
        return response->send();
    }
#endif
    return response->send(200, "text/plain", "");
}

void OTAHttp::onWsOpen(PsychicWebSocketClient* client) {
    print("[HTTP WS] begin %s\n", client->remoteIP().toString().c_str());
}

void OTAHttp::onWsClose(PsychicWebSocketClient* client) {
    print("[HTTP WS] end %s\n", client->remoteIP().toString().c_str());
}

esp_err_t OTAHttp::onWsFrame(PsychicWebSocketRequest* request, httpd_ws_frame* frame) {
#ifdef ROBOT_HAS_SCRIPT
    if (frame->len) {
        std::string content;
        bool finish = false;
        script.run({
            .filename = "<OTA WS>",
            .source = (char*)frame->payload,
            .length = frame->len,
            .write = [&](const uint8_t* buffer, size_t length) {
                if (buffer && !finish) {
                    content.append(reinterpret_cast<const char*>(buffer), length);
                } else {
                    finish = true;
                }
            },
        });
        while (!finish) {
            vTaskDelay(1);
        }
        httpd_ws_frame reply = {
            .final = true,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t*)content.c_str(),
            .len = content.length(),
        };
        return request->reply(&reply);
    }
#endif
    return request->reply(frame);
}

esp_err_t OTAHttp::onNotFound(PsychicRequest* request, PsychicResponse* response) {
    return response->send(404, "text/plain", "");
}

#endif
