
#include "config.h"

#ifdef ROBOT_HAS_OTA_HTTP

#include "ota_http.h"

OTAHttp otaHttp;

OTAHttp::OTAHttp() {
    server.onOpen([this](PsychicClient* client) {
        this->onOpen(client);
    });
    server.onClose([this](PsychicClient* client) {
        this->onClose(client);
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
    print("[HTTP] open #%u from %s\n", client->socket(), client->remoteIP().toString().c_str());
}

void OTAHttp::onClose(PsychicClient* client) {
    print("[HTTP] close #%u to %s\n", client->socket(), client->remoteIP().toString().c_str());
}

void OTAHttp::onWsOpen(PsychicWebSocketClient* client) {
    print("[ WS ] open #%u from %s\n", client->socket(), client->remoteIP().toString().c_str());
}

void OTAHttp::onWsClose(PsychicWebSocketClient* client) {
    print("[ WS ] close #%u to %s\n", client->socket(), client->remoteIP().toString().c_str());
}

esp_err_t OTAHttp::onWsFrame(PsychicWebSocketRequest* request, httpd_ws_frame* frame) {
    return request->reply(frame);
}

esp_err_t OTAHttp::onNotFound(PsychicRequest* request, PsychicResponse* response) {
    return response->send(404, "text/plain", "");
}

#endif
