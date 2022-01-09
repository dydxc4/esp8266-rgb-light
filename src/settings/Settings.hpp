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

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "LightingStatus.hpp"
#include "WiFiConfig.hpp"

class Settings
{
private:
    fs::FS _fs;
    LightingStatus _lightingStatus;
    WiFiConfig _wifiConfig;
    const String _filepath;

public:
    Settings(const char *settingsFilePath, fs::FS &fs = LittleFS);

    const bool loadSettingsFromFS();
    const bool saveSettingsToFS();

    inline WiFiConfig &getWiFiConfig() { return _wifiConfig; }
    inline LightingStatus &getLightingStatus() { return _lightingStatus; }

    void getWiFiConfigJSON(JsonVariant &jsonRoot, bool includePassword = false);
    void getLightingStatusJSON(JsonVariant &jsonRoot);

    void loadDefaultWiFiConfig();
    void loadDefaultLightingStatus();

    const bool updateWiFiSettings(JsonVariantConst &jsonRoot, const bool notifyChanges = true);
    const bool updateWiFiSettings(AsyncWebServerRequest *request);
    const bool updateLightingStatus(JsonVariantConst &jsonRoot, const bool notifyChanges = true);
};
