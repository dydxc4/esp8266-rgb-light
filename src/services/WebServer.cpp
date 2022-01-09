// The MIT License (MIT)

// Copyright (c) 2022 Daniel Diaz

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include "WebServer.hpp"

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        // Envia la pagina de configuración de WiFi luego de que un 
        // dispositivo se conecte a la red
        request->send(LittleFS, "/www/wifi-setup.html");
    }
};

WebServerService::WebServerService(int serverPort) :
    _server(serverPort),
    _socket("/ws")
{ }

void WebServerService::init(Settings &settings)
{
    _server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
    _server.on("/get-light-state", HTTP_GET, [&settings](AsyncWebServerRequest *request) 
    {
        AsyncJsonResponse *response = new AsyncJsonResponse();
        settings.getLightingStatusJSON(response->getRoot());
        response->setLength();
        request->send(response);
    });

    _server.on("/get-wifi-config", HTTP_GET, [&settings](AsyncWebServerRequest *request)
    {
        AsyncJsonResponse *response = new AsyncJsonResponse();
        settings.getWiFiConfigJSON(response->getRoot());
        response->setLength();
        request->send(response);
    });

    _server.on("/set-wifi-config", HTTP_POST, [&settings](AsyncWebServerRequest *request)
    {
        bool result = settings.updateWiFiSettings(request);
        if (result)
        {
            request->send(LittleFS, "/hidden/wifi-setup-ok.html");
            settings.saveSettingsToFS();
        }
        else
        {
            AsyncWebServerResponse *response = nullptr;
            response = request->beginResponse(LittleFS, "/hidden/wifi-setup-fail.html");
            response->setCode(400);
            request->send(response);
        }
    });

    _server.onNotFound([](AsyncWebServerRequest *request)
    {
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/hidden/not-found.html");
        response->setCode(404);
        request->send(response);
    });

    _socket.onEvent([&settings](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = nullptr;

        switch (type) 
        {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", 
                client->id(), 
                client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
            {
                StaticJsonDocument<256> doc;
                DeserializationError error = deserializeJson(doc, data, len);
                if (error)
                {
                    Serial.print(F("JSON deserialize failed: "));
                    Serial.println(error.c_str());
                    return;
                }

                JsonVariantConst json = doc;
                settings.updateLightingStatus(json);
            }
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
        }
    });

    if (WiFi.getMode() != WIFI_STA)
    {
        _dnsServer.start(53, "*", WiFi.softAPIP());
        _server.addHandler(new CaptiveRequestHandler())
            .setFilter(ON_AP_FILTER);
    }

    _server.addHandler(&_socket);
    _server.begin();
    Serial.println("WebServer started!");
}

void WebServerService::run()
{
    if (WiFi.getMode() != WIFI_STA)
    {
        _dnsServer.processNextRequest();
    }

    _socket.cleanupClients();
}
