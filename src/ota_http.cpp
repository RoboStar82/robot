
#include "config.h"

#ifdef ROBOT_HAS_OTA_HTTP

#include "ota.h"

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

OTAHttp otaHttp;

OTAHttp::OTAHttp() {}

OTAHttp::~OTAHttp() {}

void OTAHttp::begin() {
    if (!server && !httpd_start(&server, &config)) {
        config.open_fn = onOpen;
        config.close_fn = onClose;
        config.uri_match_fn = httpd_uri_match_wildcard;
        configScript.handler = onScript;
        configWebSocket.handler = onWebSocket;
        httpd_register_uri_handler(server, &configScript);
        httpd_register_uri_handler(server, &configWebSocket);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, on404);
        IPAddress ip = ota.getIP();
        print("[HTTP] curl http://%s\n", ip.toString().c_str());
        print("[HTTP] wscat -c ws://%s/ws\n", ip.toString().c_str());
    }
}

void OTAHttp::end() {
    if (server) {
        httpd_stop(server);
        server = nullptr;
    }
}

esp_err_t OTAHttp::onOpen(int fd) {
    char remoteIP[INET6_ADDRSTRLEN];
    struct sockaddr_in6 addr;
    socklen_t addrSize = sizeof(addr);
    if (getpeername(fd, (struct sockaddr*)&addr, &addrSize) >= 0) {
        inet_ntop(AF_INET, &addr.sin6_addr.un.u32_addr[3], remoteIP, sizeof(remoteIP));
        print("[HTTP] begin: %s\n", remoteIP);
    } else {
        print("[HTTP] begin: %d\n", fd);
    }
    return ESP_OK;
}

void OTAHttp::onClose(int fd) {
    print("[HTTP] end\n");
}

esp_err_t OTAHttp::onScriptPost(httpd_req_t* request) {
    httpd_resp_set_type(request, "text/plain");
    if (!request->content_len) {
        return httpd_resp_send(request, nullptr, 0);
    }
    size_t length = request->content_len;
    if (length > 8192) {
        return httpd_resp_send_err(request, HTTPD_413_CONTENT_TOO_LARGE, nullptr);
    }
    char* source = (char*)malloc(length + 1);
    if (!source) {
        return httpd_resp_send_500(request);
    }
    size_t index = 0;
    while (index < length) {
        int size = httpd_req_recv(request, source + index, length - index);
        if (size > 0) {
            index += size;
        } else if (size == HTTPD_SOCK_ERR_TIMEOUT) {
            continue;
        } else {
            free(source);
            return httpd_resp_send_500(request);
        }
    }
    source[length] = '\0';
#ifdef ROBOT_HAS_SCRIPT
    std::string content;
    bool finish = false;
    script.run({
        .filename = "<HTTP>",
        .source = source,
        .length = length,
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
    free(source);
    return httpd_resp_send(request, content.c_str(), content.length());
#endif
    free(source);
    return httpd_resp_send(request, nullptr, 0);
}

esp_err_t OTAHttp::onWebSocketGet(httpd_req_t* request) {
    if (request->method == HTTP_GET) {
        return ESP_OK;
    }
    esp_err_t r;
    httpd_ws_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    if (r = httpd_ws_recv_frame(request, &frame, 0)) {
        return r;
    }
    httpd_ws_frame reply = {
        .final = true,
        .fragmented = false,
        .type = frame.type == HTTPD_WS_TYPE_PING ? HTTPD_WS_TYPE_PONG : HTTPD_WS_TYPE_TEXT,
        .payload = nullptr,
        .len = 0,
    };
    size_t length = frame.len;
    if (!length) {
        return httpd_ws_send_frame(request, &reply);
    }
    if (length > 8192) {
        reply.type = HTTPD_WS_TYPE_CLOSE;
        return httpd_ws_send_frame(request, &reply);
    }
    char* source = (char*)malloc(length + 1);
    if (!source) {
        reply.type = HTTPD_WS_TYPE_CLOSE;
        return httpd_ws_send_frame(request, &reply);
    }
    frame.payload = (uint8_t*)source;
    if (r = httpd_ws_recv_frame(request, &frame, length)) {
        free(source);
        return r;
    }
    source[length] = '\0';
    if (frame.type != HTTPD_WS_TYPE_TEXT && frame.type != HTTPD_WS_TYPE_BINARY) {
        reply.payload = frame.payload;
        reply.len = frame.len;
        esp_err_t r = httpd_ws_send_frame(request, &reply);
        free(source);
        return r;
    }
#ifdef ROBOT_HAS_SCRIPT
    std::string content;
    bool finish = false;
    script.run({
        .filename = "<HTTP>",
        .source = source,
        .length = length,
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
    free(source);
    reply.payload = (uint8_t*)content.c_str();
    reply.len = content.length();
    return httpd_ws_send_frame(request, &reply);
#endif
    r = httpd_ws_send_frame(request, &reply);
    free(source);
    return r;
}

esp_err_t OTAHttp::onNotFound(httpd_req_t* request) {
    httpd_resp_set_type(request, "text/plain");
    return httpd_resp_send_404(request);
}

esp_err_t OTAHttp::onOpen(httpd_handle_t server, int fd) {
    return otaHttp.onOpen(fd);
}
void OTAHttp::onClose(httpd_handle_t server, int fd) {
    return otaHttp.onClose(fd);
}

esp_err_t OTAHttp::onScript(httpd_req_t* request) {
    return otaHttp.onScriptPost(request);
}

esp_err_t OTAHttp::onWebSocket(httpd_req_t* request) {
    return otaHttp.onWebSocketGet(request);
}

esp_err_t OTAHttp::on404(httpd_req_t* request, httpd_err_code_t e) {
    return otaHttp.onNotFound(request);
}

#endif
