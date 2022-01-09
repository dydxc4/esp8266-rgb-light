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

#include "bases/IBaseConfig.hpp"

class Settings;

static const uint32_t FADE_FREQUENCY_MAX = 500;
static const uint32_t FADE_FREQUENCY_MIN = 1;
static const uint32_t FADE_FREQUENCY_DEFAULT = 10;
static const uint32_t SOLID_COLOR_DEFAULT = 0xFFFFFF;
static const uint32_t FADE_COLOR_1_DEFAULT = 0x0;
static const uint32_t FADE_COLOR_2_DEFAULT = 0xFFFFFF;

enum lighting_mode_t
{
    LIGHTING_MODE_SOLID_COLOR,
    LIGHTING_MODE_RAINBOW,
    LIGHTING_MODE_FADE
};

class LightingStatus : public IBaseConfig
{
private:
    friend class Settings;

    bool _isActive;
    lighting_mode_t _mode;
    float _brightness;
    uint32_t _solidColor, _fadeColor1, _fadeColor2;
    uint32_t _rainbowFrequency, _fadeFrequency;

public:
    LightingStatus() : 
        _isActive(false), 
        _mode(LIGHTING_MODE_SOLID_COLOR), 
        _brightness(100.0),
        _solidColor(SOLID_COLOR_DEFAULT),
        _fadeColor1(0),
        _fadeColor2(SOLID_COLOR_DEFAULT),
        _rainbowFrequency(FADE_FREQUENCY_DEFAULT),
        _fadeFrequency(FADE_FREQUENCY_DEFAULT)
    { }

    inline bool isActive()
    {
        return _isActive;
    }

    inline lighting_mode_t getMode()
    {
        return _mode;
    }

    inline float getBrightness()
    {
        return _brightness;
    }

    uint8_t getBrightness_u8()
    {
        return uint8_t(_brightness * 2.55);
    }

    inline uint32_t getSolidColor()
    {
        return _solidColor;
    }

    inline uint32_t getRainbowFrequency()
    {
        return _rainbowFrequency;
    }

    inline uint32_t getFadeFrequency()
    {
        return _fadeFrequency;
    }

    inline uint32_t getFadeColor1()
    {
        return _fadeColor1;
    }

    inline uint32_t getFadeColor2()
    {
        return _fadeColor2;
    }
};
