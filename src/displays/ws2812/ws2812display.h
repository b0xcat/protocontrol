#ifndef __WS2812_PAINTER_H__
#define __WS2812_PAINTER_H__

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <initializer_list>
#include <Vector.h>

#include "ws2812matrix.h"

static const uint32_t max_matrices = 64;

class WS2812Display : public Adafruit_GFX {
private:
    WS2812Matrix _matrices[max_matrices]; 
    Vector<WS2812Matrix> matrices;

    uint _mat_boundaries[max_matrices];
    Vector<uint> mat_boundaries;

    CRGB* pixels;

    int32_t lookupMatrixIndex(uint32_t row) {
        uint32_t cur_idx = 0;
        for(auto cur_boundary : mat_boundaries) {
            if (row <= cur_boundary){ // or < ?
                return cur_idx;
            } else {
                cur_idx += 1;
            }
        }
        return -1;
    }

public:

    /**
     * Do not allow default constructor
     */
    WS2812Display() = delete;

    /**
     * Construct a WS2812Display with an initializer list of WS2812Matrix objects
     * 
     * The display width of the WS2812Display will be equal to the sum of the width of the
     * matrix objects
     * 
     * The display height will be equal to the max height of the matrices.
     * 
     * So essentially this will represent a bounding rectangle around the input matrices
     * laid out in one long row
     */
    WS2812Display (std::initializer_list<WS2812Matrix> mats): Adafruit_GFX {
        std::accumulate(mats.begin(), mats.end(),
                        0, [](int16_t a, WS2812Matrix &mat) {
                            return a + mat.width();
                        }),
        std::accumulate(mats.begin(), mats.end(),
                        0, [](int16_t a, WS2812Matrix &mat) {
                            return std::max(a, mat.height());
                        }),
    } {
        // Keep track off the added matrices
        matrices.setStorage(_matrices);
        matrices.fill(mats);

        // Calculate their (row) boundaries so we know what matrix to write to later
        mat_boundaries.setStorage(_mat_boundaries);
        uint cur_boundary = 0;
        for (auto& matrix : matrices) {
            cur_boundary += matrix.width();
            mat_boundaries.push_back(cur_boundary);
        }

        // Setup FastLED stuff
        uint32_t num_pixels = std::accumulate(mats.begin(), mats.end(), 0, 
                                [](uint32_t n_pixels, WS2812Matrix &mat) {
                                    return n_pixels + ((uint32_t)mat.width() * (uint32_t)mat.height());
                                });
        pixels = new CRGB[num_pixels];
        uint32_t cur_pixel = 0;
        for (auto& matrix : matrices) {
            matrix.fastled_mem = &pixels[cur_pixel];
            cur_pixel += (uint32_t)matrix.height() * (uint32_t)matrix.width();
        }
    }

    ~WS2812Display() {
        delete[] pixels;
    }

    void drawPixel(uint16_t x, uint16_t y, uint16_t color) {
        // Serial.println("DRAWING PIXEL");
        // Check what matrix we should draw to
        int32_t matidx = lookupMatrixIndex(x);
        
        if (matidx < 0) {
            return;
        }
  
        // TODO: COLOR SUPPORT
        if (color != 0) {
            color = 0xF800;
        }

        // Look up the lower boundary for this matrix
        uint prev_boundary = 0;
        if (matidx > 0 ){
            prev_boundary = mat_boundaries[matidx - 1];
        } 

        matrices[matidx].drawPixel(x - prev_boundary, y, color);
    }

    void show() {
        // Call FastLED.show() in main loop
    }

    void clear() {
        for (auto &matrix: matrices) {
            matrix.fillScreen(0);
        }
    }

    CRGB* getBuffer() {
        return pixels;
    }

};

#endif