#ifndef __FASTLED_STRING_H__
#define __FASTLED_STRING_H__

#include <FastLED.h>
#include <initializer_list>

#include "fastledmatrix.h"
#include "ipixel.h"

static const uint32_t max_matrices = 8;

class FastLEDString {
    friend class FastLEDDisplay;

protected: 
    std::unique_ptr<CRGB[]> pixels;
    uint32_t num_pixels;

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

    virtual void clear() {
        memset(pixels.get(), 0, sizeof(CRGB) * num_pixels);
    }

    const std::vector<std::unique_ptr<FastLEDMatrix>>& getMatrices() {
        return matrices;
    }

    virtual void show() {
        controller->showLeds(brightness);
    }

    void setBrightness(uint8_t scale) {
        brightness = scale;
    }
};

class DoubleBufferedFastLEDString: public FastLEDString {
private:
    std::unique_ptr<CRGB[]> pixels2;

    bool swapped = false;

public: 
    DoubleBufferedFastLEDString (
        CLEDController* controller,
        std::initializer_list<FastLEDMatrix*> mats
    )
    : FastLEDString(controller, mats)
    {
        pixels2 = std::unique_ptr<CRGB[]>(new CRGB[num_pixels]);
    }

    void swap() {
        uint32_t cur_pixel = 0;
        if (swapped) {
            for (auto& matrix : matrices) {
                matrix->fastled_mem = &pixels2[cur_pixel];
                cur_pixel += (uint32_t)matrix->getHeight() * (uint32_t)matrix->getWidth();
            }
            controller->setLeds(pixels2.get(), num_pixels);
        } else {
            for (auto& matrix : matrices) {
                matrix->fastled_mem = &pixels[cur_pixel];
                cur_pixel += (uint32_t)matrix->getHeight() * (uint32_t)matrix->getWidth();
            }
            controller->setLeds(pixels.get(), num_pixels);
        }
        swapped = !swapped;
    }


    void clear() override {
        if (swapped) {
            memset(pixels2.get(), 0, sizeof(CRGB) * num_pixels);
        } else {
            memset(pixels.get(), 0, sizeof(CRGB) * num_pixels);
        }
    }
};

#endif