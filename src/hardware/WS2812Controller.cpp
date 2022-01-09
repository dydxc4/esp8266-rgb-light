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

#include "WS2812Controller.hpp"

WS2812Controller::WS2812Controller(Adafruit_NeoPixel &neopixels) : 
    _neoPixels(&neopixels), _lightingStatus(nullptr)
{ }

void WS2812Controller::showRainbow()
{
    uint32_t frequency = _lightingStatus->getRainbowFrequency();
    if (frequency == 0)
    {
        return;
    }

    static uint16 hue = 0;
    static unsigned long lastTime = 0;
    unsigned long time = millis();
    uint32_t interval_ms = 1000 / frequency;
    
    if ((time - lastTime) > interval_ms)
    {
        uint32_t color = Adafruit_NeoPixel::ColorHSV(hue);
        color = Adafruit_NeoPixel::gamma32(color);
        _neoPixels->fill(color);
        _neoPixels->show();

        hue += 16;
        lastTime = millis();
    }
}
    
void WS2812Controller::showSolidColor()
{ 
    _neoPixels->fill(_lightingStatus->getSolidColor());
    _neoPixels->show();
}

void WS2812Controller::showFade()
{

}

void WS2812Controller::init(LightingStatus &status)
{
    _lightingStatus = &status;
    _neoPixels->begin();

    if (_lightingStatus->isActive() && _lightingStatus->getMode() == LIGHTING_MODE_SOLID_COLOR)
    {
        _neoPixels->setBrightness(_lightingStatus->getBrightness_u8());
        _neoPixels->fill(_lightingStatus->getSolidColor());
    }
    else
    {
        _neoPixels->clear();
    }
    
    _neoPixels->show();
}

bool WS2812Controller::update()
{
    if (_lightingStatus == nullptr)
    {
        return false;
    }

    bool result = _lightingStatus->hasChanged();
    if (result)
    {
        Serial.println(F("Updating lighting..."));
        uint8_t brightness_u8 = _lightingStatus->getBrightness_u8();

        if (brightness_u8 == 0 || !_lightingStatus->isActive())
        {
            Serial.println(F("Lights OFF"));
            _neoPixels->clear();
            _neoPixels->show();
        }
        else
        {
            Serial.printf("Brightness %.02f%%\n", _lightingStatus->getBrightness());
            _neoPixels->setBrightness(brightness_u8);

            switch (_lightingStatus->getMode())
            {
            case LIGHTING_MODE_RAINBOW:
                Serial.printf("Lights ON with rainbow at %u Hz\n", _lightingStatus->getRainbowFrequency());
                //showRainbow();
                break;
            case LIGHTING_MODE_FADE:
                Serial.printf("Lights ON with fade at %u Hz\n", _lightingStatus->getFadeFrequency());
                //showFade();
                break;
            default:
                Serial.printf("Lights ON with solid color #%06X\n", _lightingStatus->getSolidColor());
                showSolidColor();
                break;
            }
        }
    }
    else if (_lightingStatus->isActive())
    {
        if (_lightingStatus->getMode() == LIGHTING_MODE_RAINBOW)
        {
            showRainbow();
        }
        else if (_lightingStatus->getMode() == LIGHTING_MODE_FADE)
        {
            showFade();
        }
    }

    return result;
}
