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

#pragma once

#include "Assignments.h"
#include "DefaultValues.h"
#include "bases/IBaseConfig.hpp"
#include "bases/ILightController.hpp"
#include "hardware/WS2812Controller.hpp"
#include "services/WebServer.hpp"
#include "settings/Settings.hpp"

class IotLightController
{
private:
    ILightController *_lightController;
    WebServerService _webserver;
    Settings _settings;

    bool startWiFiSTA()
    {
        WiFiConfig &wifiConfig = _settings.getWiFiConfig();
        const IPAddress &localIP = wifiConfig.getLocalIP();
        const IPAddress &gateway = wifiConfig.getGateway();
        const IPAddress &subnetMask = wifiConfig.getSubnetMask();
        const String &ssid = wifiConfig.getSSID();
        const String &password = wifiConfig.getPassword();

        WiFi.mode(WIFI_STA);
        if (!WiFi.config(localIP, gateway, subnetMask))
        {
            Serial.println(F("STA Failed to configure"));
            return false;
        }

        WiFi.begin(ssid, password);
        Serial.printf("Connecting to %s", ssid.c_str());
        unsigned long currentMillis, initialMillis = millis();

        while (WiFi.status() != WL_CONNECTED)
        {
            currentMillis = millis();
            if (currentMillis - initialMillis >= CONNECTION_TIMEOUT_MS)
            {
                Serial.println(F("WiFi connection failed"));
                return false;
            }

            Serial.print('.');
            delay(500);
        }
        
        Serial.println();
        Serial.print(F("Connection successfully | IP: "));
        Serial.println(WiFi.localIP());
        return true;
    }

    bool startWiFiAP()
    {
        bool result;
        Serial.println(F("Setting AP..."));
        WiFi.mode(WIFI_AP);
        result = WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASSWORD);
        if (result)
        {
            Serial.print(F("SSID: "));
            Serial.println(WiFi.softAPSSID());
            Serial.print(F("IP: "));
            Serial.println(WiFi.softAPIP());
        }

        return result;
    }

public:
    IotLightController() : 
        _lightController(nullptr),
        _settings(SETTINGS_FILE)
    { }

    bool init(ILightController &lightController)
    {
        _lightController = &lightController;
        bool result = LittleFS.begin();
        if (result)
        {
            result = _settings.loadSettingsFromFS();
        }
        
        if (!result)
        {
            _settings.loadDefaultWiFiConfig();
        }

        LightingStatus *lighting = &_settings.getLightingStatus();
        _lightController->init(*lighting);
        if (!result || !startWiFiSTA())
        {
            startWiFiAP();
        }
            
        _webserver.init(_settings);
        return result;
    }

    void run()
    {
        _webserver.run();

        if (_lightController->update())
        {
            Serial.println("Settings updated");
            _settings.saveSettingsToFS();
        }

        if (_settings.getWiFiConfig().hasChanged())
        {
            Serial.println("WiFi config updated");
            for (int i = 3 - 1; i >= 0; i--)
            {
                Serial.printf("Restarting in %d seconds...\n", i);
                delay(1000);
            }

            ESP.restart();
        }
    }
};
