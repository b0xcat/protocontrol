#ifndef __WS2812_MATRIX_H__
#define __WS2812_MATRIX_H__


#include <Adafruit_GFX.h>
#include <FastLED.h>
#include "colorconversion.h"

#define MAT_W 8
#define MAT_H 8

class WS2812Matrix : public Adafruit_GFX {
    friend class WS2812String;
private:
    CRGB *fastled_mem;

    // Private default constructor, should not be used externally
    WS2812Matrix(): Adafruit_GFX(0,0) {}

public:


    WS2812Matrix(int16_t width, int16_t height, uint8_t rotation) : Adafruit_GFX(width, height) {
        this->setRotation(rotation);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        //Serial.println("DRAWING PIXEL IN WS MATRIX");
        
        // Bounds check
        if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
            return;
        }

        // TODO: COLOR SUPPORT
        if (color != 0) {
            color = 0xF800;
        }

        // What "physical" matrix are we in?
        uint n_mats = WIDTH / MAT_W;
        uint matindex = x / MAT_W;
        if (rotation == 2) {
            matindex = n_mats - matindex - 1;
        }
        
        // Handle rotation cases (thx adafruit gfx)
        int16_t t;
        switch (rotation) {
            case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
            case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
            case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
        }
        
        uint n_skip = (MAT_W * MAT_H * matindex);
        x = x % MAT_W;
        y = y % MAT_H;
        
        // Every second row, we need to invert the X coordinate due to how the matrices are wired
        if (y % 2 == 1) {
            x = MAT_W - x - 1;
        }
        fastled_mem[n_skip + x + y * MAT_H] = CRGB(expandColor(color));

    }

};

#endif