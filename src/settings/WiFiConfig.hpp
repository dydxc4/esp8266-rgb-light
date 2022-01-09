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

#include <IPAddress.h>
#include <WString.h>
#include "bases/IBaseConfig.hpp"

class Settings;

class WiFiConfig : public IBaseConfig
{
private:
    friend class Settings;

    String _ssid, _password;
    IPAddress _localIP, _gatewayIP, _subnetMask;

    bool replace(String &curruent, const char *newValue)
    {
        if (newValue != nullptr)
        {
            curruent.clear();
            curruent.concat(newValue);
            return true;
        }
        return false;
    }

    bool replace(IPAddress &current, const char *newValue)
    {
        bool result = newValue != nullptr && IPAddress::isValid(newValue);
        if (result)
        {
            current.clear();
            current.fromString(newValue);
        }
        return result;
    }

    bool replaceSSID(const char *value)
    {
        bool result = value != nullptr;
        if (result)
        {
            size_t len = strlen(value);
            result = len > 0 && len <= 32;
            if (result)
            {
                replace(_ssid, value);
            }
        }
        return result;
    }

public:

    inline const String &getSSID() { return _ssid; }
    inline const String &getPassword() { return _password; }
    inline const IPAddress &getLocalIP() { return _localIP; }
    inline const IPAddress &getGateway() { return _gatewayIP; }
    inline const IPAddress &getSubnetMask() { return _subnetMask; }
};
