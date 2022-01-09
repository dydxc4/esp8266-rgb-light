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

#include "Settings.hpp"
#include "SettingsNames.hpp"

Settings::Settings(const char *settingsFilePath, fs::FS &fs) : 
    _fs(fs), _filepath(settingsFilePath)
{ }

const bool Settings::loadSettingsFromFS()
{
    bool result = _fs.exists(_filepath);
    if (result)
    {
        File file = _fs.open(_filepath, "r");
        StaticJsonDocument<768> json;
        DeserializationError error = deserializeJson(json, file);

        if (error)
        {
            Serial.print(F("JSON deserialize failed: "));
            Serial.println(error.c_str());
            result = false;
        }
        else
        {
            JsonVariantConst jsonLighting = json[JSON_LIGHTING_STATUS];
            JsonVariantConst jsonWiFi = json[JSON_WIFI];
            result = updateWiFiSettings(jsonWiFi, false) && updateLightingStatus(jsonLighting, false);
        }

        file.close();
    }
    else
    {
        Serial.println(F("Settings file not found"));
    }

    if (result)
    {
        Serial.println(F("Settings successfully loaded!"));
    }
    else
    {
        Serial.println(F("Failure to load settings"));
    }

    return result;
}

const bool Settings::saveSettingsToFS()
{
    bool result = false;
    File file = LittleFS.open(_filepath, "w");
    if (file)
    {
        StaticJsonDocument<384> json;

        JsonVariant jsonwifi = json.createNestedObject(JSON_WIFI);
        JsonVariant jsonLightStatus = json.createNestedObject(JSON_LIGHTING_STATUS);
        getWiFiConfigJSON(jsonwifi, true);
        getLightingStatusJSON(jsonLightStatus);
        serializeJson(json, file);
        file.close();
        Serial.println(F("Settings successfully saved"));
        result = true;
    }

    return result;
}

void Settings::getWiFiConfigJSON(JsonVariant &jsonRoot, bool includePassword)
{
    jsonRoot[JSON_SSID] = _wifiConfig._ssid;
    jsonRoot[JSON_LOCAL_IP] = _wifiConfig._localIP.toString();
    jsonRoot[JSON_GATEWAY] = _wifiConfig._gatewayIP.toString();
    jsonRoot[JSON_SUBNET_MASK] = _wifiConfig._subnetMask.toString();

    if (includePassword)
    {
        jsonRoot[JSON_PASSWORD] = _wifiConfig._password;
    }
}

void Settings::getLightingStatusJSON(JsonVariant &jsonRoot)
{
    jsonRoot[JSON_IS_ACTIVE] = _lightingStatus._isActive;
    jsonRoot[JSON_LIGHTING_MODE] = _lightingStatus._mode;
    jsonRoot[JSON_BRIGHTNESS] = _lightingStatus._brightness;
    jsonRoot[JSON_SOLID_COLOR][JSON_COLOR] = _lightingStatus._solidColor;
    jsonRoot[JSON_RAINBOW][JSON_FREQUENCY] = _lightingStatus._rainbowFrequency;
    jsonRoot[JSON_FADE][JSON_FREQUENCY] = _lightingStatus._fadeFrequency;
    jsonRoot[JSON_FADE][JSON_COLOR_1] = _lightingStatus._fadeColor1;
    jsonRoot[JSON_FADE][JSON_COLOR_2] = _lightingStatus._fadeColor2;
}

void Settings::loadDefaultWiFiConfig()
{
    _wifiConfig._ssid.clear();
    _wifiConfig._password.clear();
    _wifiConfig.replace(_wifiConfig._localIP, "192.168.0.100");
    _wifiConfig.replace(_wifiConfig._gatewayIP, "192.168.0.1");
    _wifiConfig.replace(_wifiConfig._subnetMask, "255.255.255.0");
}

void Settings::loadDefaultLightingStatus()
{
    _lightingStatus._isActive = false;
    _lightingStatus._mode = LIGHTING_MODE_SOLID_COLOR;
    _lightingStatus._brightness = 100.0;
    _lightingStatus._solidColor = SOLID_COLOR_DEFAULT;
    _lightingStatus._fadeColor1 = 0;
    _lightingStatus._fadeColor2 = SOLID_COLOR_DEFAULT;
    _lightingStatus._rainbowFrequency = FADE_FREQUENCY_DEFAULT;
    _lightingStatus._fadeFrequency = FADE_FREQUENCY_DEFAULT;
}

const bool Settings::updateWiFiSettings(JsonVariantConst &jsonRoot, const bool notifyChanges)
{
    bool result = _wifiConfig.replaceSSID(jsonRoot[JSON_SSID]) &&
        _wifiConfig.replace(_wifiConfig._password, jsonRoot[JSON_PASSWORD]) &&
        _wifiConfig.replace(_wifiConfig._localIP, jsonRoot[JSON_LOCAL_IP]) &&
        _wifiConfig.replace(_wifiConfig._gatewayIP, jsonRoot[JSON_GATEWAY]) &&
        _wifiConfig.replace(_wifiConfig._subnetMask, jsonRoot[JSON_SUBNET_MASK]);

    if (result && notifyChanges)
    {
        _wifiConfig._hasChanged = true;
    }

    return result;
}

const bool Settings::updateWiFiSettings(AsyncWebServerRequest *request)
{
    bool result = request != nullptr;
    if (result && request->hasArg(JSON_SSID))
    {
        AsyncWebParameter *ssid = request->getParam(JSON_SSID, true);
        result &= _wifiConfig.replaceSSID(ssid->value().c_str());
    }

    if (result)
    {
        AsyncWebParameter *password = request->getParam(JSON_PASSWORD, true);
        const char *passwordValue = password != nullptr ? password->value().c_str() : "";
        result &= _wifiConfig.replace(_wifiConfig._password, passwordValue);
    }

    if (result && request->hasArg(JSON_LOCAL_IP))
    {
        AsyncWebParameter *localIP = request->getParam(JSON_LOCAL_IP, true);
        result &= _wifiConfig.replace(_wifiConfig._localIP, localIP->value().c_str());
    }

    if (result && request->hasArg(JSON_GATEWAY))
    {
        AsyncWebParameter *gateway = request->getParam(JSON_GATEWAY, true);
        result &= _wifiConfig.replace(_wifiConfig._gatewayIP, gateway->value().c_str());
    }

    if (result && request->hasArg(JSON_SUBNET_MASK))
    {
        AsyncWebParameter *subnetMask = request->getParam(JSON_SUBNET_MASK, true);
        result &= _wifiConfig.replace(_wifiConfig._subnetMask, subnetMask->value().c_str());
    }

    if (result)
    {
        _wifiConfig._hasChanged = true;
    }

    return result;
}

const bool Settings::updateLightingStatus(JsonVariantConst &jsonRoot, const bool notifyChanges)
{
    // Lee los valores que requeridos por LightingState
    _lightingStatus._isActive = jsonRoot[JSON_IS_ACTIVE];
    _lightingStatus._mode = jsonRoot[JSON_LIGHTING_MODE];
    _lightingStatus._solidColor = jsonRoot[JSON_SOLID_COLOR][JSON_COLOR];
    _lightingStatus._fadeColor1 = jsonRoot[JSON_FADE][JSON_COLOR_1];
    _lightingStatus._fadeColor2 = jsonRoot[JSON_FADE][JSON_COLOR_2];
    float brightness = jsonRoot[JSON_BRIGHTNESS];
    uint32_t fadeFrequency = jsonRoot[JSON_FADE][JSON_FREQUENCY];
    uint32_t rainbowFrequency = jsonRoot[JSON_RAINBOW][JSON_FREQUENCY];

    // valida y restringe los valores de frecuencia y brillo
    _lightingStatus._brightness = constrain(brightness, 0, 100);
    _lightingStatus._rainbowFrequency = constrain(rainbowFrequency, FADE_FREQUENCY_MIN, FADE_FREQUENCY_MAX);
    _lightingStatus._fadeFrequency = constrain(fadeFrequency, FADE_FREQUENCY_MIN, FADE_FREQUENCY_MAX);

    if (notifyChanges)
    {
        _lightingStatus._hasChanged = true;
    }
    
    return true;
}
