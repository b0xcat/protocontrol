#ifndef __FASTLED_DISPLAY_H__
#define __FASTLED_DISPLAY_H__

#include <etl/vector.h>
#include <FastLED.h>
#include <initializer_list>
#include <Vector.h>

#include "displays/fastled/fastledstring.h"

#include "ipixel.h"


class FastLEDDisplay : public IPixelReadWriteable {
private:
    std::vector<std::unique_ptr<FastLEDString>> _strings;

    uint16_t _width;
    uint16_t _height;

public:
    /**
     * Do not allow default constructor
     */
    FastLEDDisplay() = delete;

    /**
     * Construct a WS2812Display with an initializer list of WS2812String objects
     */
    FastLEDDisplay (std::initializer_list<FastLEDString*> strings) {

        _strings.reserve(strings.size());

        // Keep track off the added matrices
        for (auto &string : strings) {
            _strings.push_back(std::unique_ptr<FastLEDString>(string));
        }

    }

    void show() {
        // FastLED.setBrightness(16);
        unsigned long before = micros();
        for (auto &matrixString: _strings) {
            matrixString->show();
        }
        // FastLED.show();
        unsigned long after = micros();
        // Serial.print("FASTLED TOOK: ");
        // Serial.print(after - before);
        // Serial.println(" us");
    }

    void setBrightness(uint16_t display_idx, uint8_t scale) {
        _strings.at(display_idx)->setBrightness(scale);
    }

    void setBrightness(uint8_t scale) {
        for (auto& string: _strings) {
            string->setBrightness(scale);
        }
    }

    void setPixel(uint16_t x, uint16_t y, CRGB color) {
        if (color == CRGB {0, 0, 0}) {
            return;
        }

        uint stringn = 0;
        uint matn = 0;
        for (auto& string: _strings) {
            for (auto& matrix: string->getMatrices()) {
                auto matrix_x = matrix->getX();
                auto matrix_y = matrix->getY();
                auto matrix_w = matrix->getWidth();
                auto matrix_h = matrix->getHeight();

                if (
                    x >= matrix_x 
                    && y >= matrix_y 
                    && x < matrix_x + matrix_w 
                    && y < matrix_y + matrix_h
                ) {
                    // Serial.printf("Setting pixel in str %d, mat %d (%d, %d) -> (%d, %d, %d) \n", stringn, matn, x, y, color.r, color.g, color.b);
                    matrix->setPixel(x - matrix_x, y - matrix_y, color);
                }
                matn++;
            }
            stringn++;
        }
    }

    // TODO:
    CRGB getPixel(uint16_t x, uint16_t y) const { return {1,2,3}; }
    uint16_t getWidth() const { return 1; }
    uint16_t getHeight() const { return 1; }

    void clear() {
        for (auto &matrixString: _strings) {
            matrixString->clear();
        }
    }

    void fill(CRGB color) {
        for (auto &matrixString: _strings) {
            for (uint32_t n = 0; n < matrixString->num_pixels; n++) {
                matrixString->pixels[n] = color;
            }
        }
    }

    void swap() {
        for (auto &matrixString: _strings) {
            
            DoubleBufferedFastLEDString* dbfls = 
                dynamic_cast<DoubleBufferedFastLEDString*>(matrixString.get());

            if (dbfls) {
                dbfls->swap();
            }
        }
    }

};


#endif