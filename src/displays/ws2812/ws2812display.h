#ifndef __WS2812_DISPLAY_H__
#define __WS2812_DISPLAY_H__

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <initializer_list>
#include <Vector.h>

#include "ws2812string.h"

static const uint32_t max_matrix_strings = 8;

class WS2812Display : public Adafruit_GFX {
private:
    std::vector<std::unique_ptr<WS2812String>> matrix_strings;
    
    // std::unique_ptr<WS2812String> _matrix_strings[max_matrix_strings]; 
    // Vector<std::unique_ptr<WS2812String>> matrix_strings;

    uint _string_boundaries[max_matrices];
    Vector<uint> string_boundaries;

    uint8_t brightness {32};
    
    int32_t lookupStringIndex(uint32_t row) {
        uint32_t cur_idx = 0;
        for(auto cur_boundary : string_boundaries) {
            if (row < cur_boundary){ // or < ?
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
    // WS2812Display(std::initializer_list<WS2812String> ints) : Adafruit_GFX(0, 0) {
    //     Serial.println("DEFAULT DISPLAY CONSTRUCTOR");
    // }

    /**
     * Construct a WS2812Display with an initializer list of WS2812String objects
     * 
     * The display width of the WS2812Display will be equal to the sum of the width of the
     * matrix objects in the strings
     * 
     * The display height will be equal to the max height of the matrices.
     * 
     * So essentially this will represent a bounding rectangle around the input matrices
     * laid out in one long row
     */
    WS2812Display (std::initializer_list<WS2812String*> strings): Adafruit_GFX {
        std::accumulate(strings.begin(), strings.end(),
                        (int16_t)0, [](int16_t a, const WS2812String* mat) {
                            return a + mat->width();
                        }),
        std::accumulate(strings.begin(), strings.end(),
                        (int16_t)0, [](int16_t a, const WS2812String* mat) {
                            return std::max(a, mat->height());
                        }),
    } {
        Serial.println("IN DISPLAY CONSTRUCTOR");
        // Keep track off the added matrices
        // matrix_strings.setStorage(_matrix_strings);
        Serial.println("IN DISPLAY CONSTRUCTOR set storage");

        for (auto &string : strings) {
            matrix_strings.push_back(std::unique_ptr<WS2812String>(string));
        }
        Serial.println("IN DISPLAY CONSTRUCTOR push back strings");

        // Calculate their (row) boundaries so we know what string to write to later
        string_boundaries.setStorage(_string_boundaries);
        Serial.println("IN DISPLAY CONSTRUCTOR set storage boundaries");
        uint cur_boundary = 0;
        for (auto& matrix_string : matrix_strings) {
            cur_boundary += matrix_string->width();
            string_boundaries.push_back(cur_boundary);
        }
        Serial.println("IN DISPLAY CONSTRUCTOR loop boundaries");
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        int32_t stringidx = lookupStringIndex(x);
        
        if (stringidx < 0) {
            return;
        }

        uint prev_boundary = 0;
        if (stringidx > 0 ){
            prev_boundary = string_boundaries[stringidx - 1];
        } 

        matrix_strings[stringidx]->drawPixel(x - prev_boundary, y, color);
    }

    void show() {
        Serial.println("Display show called");
        for (auto &matrixString: matrix_strings) {
            Serial.println("MATRIXSTRING SHOW");
            matrixString->show(brightness);
        }
    }

    void setBrightness(uint8_t scale) {
        brightness = scale;
    }

    void clear() {
        for (auto &matrixString: matrix_strings) {
            matrixString->fillScreen(0);
        }
    }

};

#endif