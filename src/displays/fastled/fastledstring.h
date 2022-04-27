#ifndef __FASTLED_STRING_H__
#define __FASTLED_STRING_H__

#include <FastLED.h>
#include <initializer_list>
#include <Vector.h>

#include "fastledmatrix.h"
#include "ipixel.h"

static const uint32_t max_matrices = 8;

class FastLEDString {
    friend class FastLEDDisplay;

protected: 
    std::unique_ptr<CRGB[]> pixels;
    uint32_t num_pixels;

private:
    std::vector<std::unique_ptr<FastLEDMatrix>> matrices;
    std::vector<uint> mat_boundaries;
    std::unique_ptr<CLEDController> controller;

    uint8_t brightness;


public:
    FastLEDString() = delete;

    /**
     * Construct a WS2812String with an initializer list of FastLEDMatrix objects
     * 
     * The display width of the WS2812String will be equal to the sum of the width of the
     * matrix objects
     * 
     */
    FastLEDString (CLEDController* controller, std::initializer_list<FastLEDMatrix*> mats) 
    : controller(controller)
    {
        // Keep track off the added matrices
        for (auto &matrix: mats) {
            matrices.push_back(std::unique_ptr<FastLEDMatrix>(matrix));
        }

        // Setup FastLED stuff
        num_pixels = std::accumulate(mats.begin(), mats.end(), 0, 
                                [](uint32_t n_pixels, const FastLEDMatrix *mat) {
                                    return n_pixels + ((uint32_t)mat->getWidth() * (uint32_t)mat->getHeight());
                                });
        pixels = std::unique_ptr<CRGB[]>(new CRGB[num_pixels]);

        FastLED.addLeds(controller, pixels.get(), num_pixels);

        // Let the matrices know about their memory
        uint32_t cur_pixel = 0;
        for (auto& matrix : matrices) {
            matrix->fastled_mem = &pixels[cur_pixel];
            cur_pixel += (uint32_t)matrix->getHeight() * (uint32_t)matrix->getWidth();
        }
    }

    void clear() {
        memset(pixels.get(), 0, sizeof(CRGB) * num_pixels);
    }

    const std::vector<std::unique_ptr<FastLEDMatrix>>& getMatrices() {
        return matrices;
    }

    void show() {
        controller->showLeds(brightness);
    }

    void setBrightness(uint8_t scale) {
        brightness = scale;
    }
};

#endif