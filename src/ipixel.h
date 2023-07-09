#ifndef __IPIXEL_HEADER_H___
#define __IPIXEL_HEADER_H___

#include <stdint.h>
#include <FastLED.h>

#include "colorconversion.h"


class IPixelBase {
public:
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
};


class IPixelReadable: public IPixelBase {
public:
    virtual CRGB getPixel(uint16_t x, uint16_t y) const = 0;
};


class IPixelReadWriteable: public IPixelReadable {
public:
    virtual void setPixel(uint16_t x, uint16_t y, CRGB color) = 0;

    // [[deprecated("We're moving to CRGB baybeeeee")]]
    virtual void setPixel(uint16_t x, uint16_t y, uint16_t color) {
        CRGB tmp;
        convert565toCRGB(color, tmp);
        this->setPixel(x, y, tmp);
    };

    virtual void clear() = 0;
};

#endif