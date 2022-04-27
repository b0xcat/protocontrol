#ifndef __CRGB_CANVAS_HEADER__
#define __CRGB_CANVAS_HEADER__

#include <stdint.h>
#include "ipixel.h"
#include <FastLED.h>

class CRGBCanvas: public IPixelReadWriteable {
private:
    uint16_t width;
    uint16_t height;
    std::unique_ptr<CRGB[]> buf;
    
public:

    CRGBCanvas (uint16_t width, uint16_t height) 
    : width(width)
    , height(height)
    , buf(new CRGB[width * height])
    {}

    CRGB getPixel(uint16_t x, uint16_t y) const {
        return buf[x * height + y];
    }

    void setPixel(uint16_t x, uint16_t y, CRGB color) {
        buf[x * height + y] = color;
    }

    void drawFastVLine(uint16_t x, uint16_t y, uint16_t height, CRGB color) {
        for (uint16_t i = 0; i < height; i++) {
            setPixel(x, y + i, color);
        }
    }

    void clear() {
        memset(buf.get(), 0, width * height * sizeof(CRGB));
    }

    uint16_t getWidth() const {
        return width;
    }

    uint16_t getHeight() const {
        return height;
    }

};

#endif