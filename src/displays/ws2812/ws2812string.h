#ifndef __WS2812_STRING_H__
#define __WS2812_STRING_H__

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <initializer_list>
#include <Vector.h>

#include "ws2812matrix.h"

static const uint32_t max_matrices = 8;

class WS2812String : public Adafruit_GFX {
    friend class WS2812Display;

protected: 
    CRGB* pixels;
    uint32_t num_pixels;

private:
    WS2812Matrix _matrices[max_matrices]; 
    Vector<WS2812Matrix> matrices;

    uint _mat_boundaries[max_matrices];
    Vector<uint> mat_boundaries;

    int32_t lookupMatrixIndex(uint32_t row) {
        uint32_t cur_idx = 0;
        for(auto cur_boundary : mat_boundaries) {
            if (row < cur_boundary){ // or < ?
                return cur_idx;
            } else {
                cur_idx += 1;
            }
        }
        return -1;
    }



public:
    WS2812String() = delete;

    /**
     * Construct a WS2812String with an initializer list of WS2812Matrix objects
     * 
     * The display width of the WS2812String will be equal to the sum of the width of the
     * matrix objects
     * 
     * The display height will be equal to the max height of the matrices.
     * 
     * So essentially this will represent a bounding rectangle around the input matrices
     * laid out in one long row
     */
    WS2812String (std::initializer_list<WS2812Matrix> mats): Adafruit_GFX {
        std::accumulate(mats.begin(), mats.end(),
                        (int16_t)0, [](int16_t a, const WS2812Matrix &mat) {
                            return a + mat.width();
                        }),
        std::accumulate(mats.begin(), mats.end(),
                        (int16_t)0, [](int16_t a, const WS2812Matrix &mat) {
                            return std::max(a, mat.height());
                        })
        }
    {
        // Keep track off the added matrices
        matrices.setStorage(_matrices);
        for (auto &matrix: mats) {
            matrices.push_back(matrix);
        }

        // Calculate their (row) boundaries so we know what matrix to write to later
        mat_boundaries.setStorage(_mat_boundaries);
        uint cur_boundary = 0;
        for (auto& matrix : matrices) {
            cur_boundary += matrix.width();
            mat_boundaries.push_back(cur_boundary);
        }

        // Setup FastLED stuff
        num_pixels = std::accumulate(mats.begin(), mats.end(), 0, 
                                [](uint32_t n_pixels, const WS2812Matrix &mat) {
                                    return n_pixels + ((uint32_t)mat.width() * (uint32_t)mat.height());
                                });
        pixels = new CRGB[num_pixels];
        uint32_t cur_pixel = 0;
        for (auto& matrix : matrices) {
            matrix.fastled_mem = &pixels[cur_pixel];
            cur_pixel += (uint32_t)matrix.height() * (uint32_t)matrix.width();
        }
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        // Serial.println("DRAWING PIXEL");
        // Check what matrix we should draw to
        int32_t matidx = lookupMatrixIndex(x);
        
        if (matidx < 0) {
            return;
        }
  
        // Look up the lower boundary for this matrix
        uint prev_boundary = 0;
        if (matidx > 0 ){
            prev_boundary = mat_boundaries[matidx - 1];
        } 

        matrices[matidx].drawPixel(x - prev_boundary, y, color);
    }

    void clear() {
        for (auto &matrix: matrices) {
            matrix.fillScreen(0);
        }
    }

    CRGB* getBuffer() {
        return pixels;
    }

    virtual void show(uint8_t brightness) {
        Serial.println("Called wrong show :(");
    }

    // TODO: figure out why uncommenting this breaks things horribly
    // For now, just pretend this is not a potential memory leak
    // ~WS2812String() {
    //     delete[] pixels;
    // }
};

template <uint8_t data_pin>
class WS2812StringPin : public WS2812String {
    friend class WS2812Display;

private:
    CLEDController *controller;

public:
    WS2812StringPin (std::initializer_list<WS2812Matrix> mats)
    : WS2812String(mats)
    {
        controller = &FastLED.addLeds<WS2812, data_pin, GRB>(pixels, num_pixels);
    }

    void show(uint8_t brightness) override {
        controller->showLeds(brightness);
    }
};

// template <uint8_t data_pin>
// WS2812StringPin<data_pin>* WS2812String(std::initializer_list<WS2812Matrix> mats) {
//     return new WS2812StringPin<data_pin>(mats);
// }

#endif