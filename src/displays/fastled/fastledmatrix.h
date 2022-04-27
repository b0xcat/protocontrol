#ifndef __FASTLED_MATRIX_H__
#define __FASTLED_MATRIX_H__

#include "ipixel.h"
#include <FastLED.h>
#include "colorconversion.h"

#define MAT_W 8
#define MAT_H 8

class FastLEDMatrix : public IPixelReadWriteable {
    friend class FastLEDString;
    friend class DoubleBufferedFastLEDString;
private:
    CRGB *fastled_mem;

    // TODO: properly implement rotation
    uint8_t _rotation;

    uint16_t _width;
    uint16_t _height;
    
    uint16_t _pos_x;
    uint16_t _pos_y;

    FastLEDMatrix() {}

public:


    FastLEDMatrix(int16_t width, int16_t height, uint16_t pos_x, uint16_t pos_y, uint8_t rotation)
    : _rotation(rotation)
    , _width(width)
    , _height(height)
    , _pos_x(pos_x)
    , _pos_y(pos_y)
    {}


    void setPixel(uint16_t x, uint16_t y, CRGB color) {
        // Bounds check
        if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
            return;
        }

        // What "physical" matrix are we in?
        uint n_mats = _width / MAT_W;
        uint matindex = x / MAT_W;
        if (_rotation == 2) {
            matindex = n_mats - matindex - 1;
        }
        
        // Handle rotation cases (thx adafruit gfx)
        int16_t t;
        switch (_rotation) {
            case 1:
            t = x;
            x = _width - 1 - y;
            y = t;
            break;
            case 2:
            x = _width - 1 - x;
            y = _height - 1 - y;
            break;
            case 3:
            t = x;
            x = y;
            y = _height - 1 - t;
            break;
        }
        
        uint n_skip = (MAT_W * MAT_H * matindex);
        x = x % MAT_W;
        y = y % MAT_H;
        
        // Every second row, we need to invert the X coordinate due to how the matrices are wired
        if (y % 2 == 1) {
            x = MAT_W - x - 1;
        }

        fastled_mem[n_skip + x + y * MAT_H] = color;

    }

    // void setPixel(uint16_t x, uint16_t y, CRGB color) {
    //     //Serial.println("DRAWING PIXEL IN WS MATRIX");
        
    //     // Bounds check
    //     if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
    //         return;
    //     }

    //     // Handle rotation cases (thx adafruit gfx)
    //     int16_t t;
    //     switch (_rotation) {
    //         case 1:
    //         t = x;
    //         x = _width - 1 - y;
    //         y = t;
    //         break;
    //         case 2:
    //         x = _width - 1 - x;
    //         y = _height - 1 - y;
    //         break;
    //         case 3:
    //         t = x;
    //         x = y;
    //         y = _height - 1 - t;
    //         break;
    //     }
        
    //     // Every second row, we need to invert the X coordinate due to how the matrices are wired
    //     if (y % 2 == 1) {
    //         x = _width - x - 1;
    //     }
    //     fastled_mem[x + y * _height] = color;
    // }

    CRGB getPixel(uint16_t x, uint16_t y) const {
        // TODO: rotation support
        return fastled_mem[x + y * _height];

    }

    uint16_t getWidth() const {
        return _width;
    }

    uint16_t getHeight() const {
        return _height;
    }

    uint16_t getX() const {
        return _pos_x;
    }

    uint16_t getY() const {
        return _pos_y;
    }

    void clear() {
        memset(fastled_mem, 0, _width * _height * sizeof(CRGB));
    }
};

#endif