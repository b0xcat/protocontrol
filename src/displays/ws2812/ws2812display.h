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
    WS2812String _matrix_strings[max_matrix_strings]; 
    Vector<WS2812String> matrix_strings;

    uint _string_boundaries[max_matrices];
    Vector<uint> string_boundaries;


    int32_t lookupStringIndex(uint32_t row) {
        uint32_t cur_idx = 0;
        for(auto cur_boundary : string_boundaries) {
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
    WS2812Display (std::initializer_list<WS2812String> strings): Adafruit_GFX {
        std::accumulate(strings.begin(), strings.end(),
                        0, [](int16_t a, WS2812String &mat) {
                            return a + mat.width();
                        }),
        std::accumulate(strings.begin(), strings.end(),
                        0, [](int16_t a, WS2812String &mat) {
                            return std::max(a, mat.height());
                        }),
    } {
        // Keep track off the added matrices
        matrix_strings.setStorage(_matrix_strings);
        matrix_strings.fill(strings);

        // Calculate their (row) boundaries so we know what string to write to later
        string_boundaries.setStorage(_string_boundaries);
        uint cur_boundary = 0;
        for (auto& matrix_string : matrix_strings) {
            cur_boundary += matrix_string.width();
            string_boundaries.push_back(cur_boundary);
        }
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

        matrix_strings[stringidx].drawPixel(x - prev_boundary, y, color);
    }

    void show() {
        // Call FastLED.show() in main loop
    }

    void clear() {
        for (auto &matrixString: matrix_strings) {
            matrixString.fillScreen(0);
        }
    }

    std::vector<std::tuple<CRGB*, ssize_t, const uint8_t>> getBuffers() {
        std::vector<std::tuple<CRGB*, ssize_t, const uint8_t>> buffers(matrix_strings.size());
        for (auto &matrix_string : matrix_strings) {
            buffers.push_back(std::tuple<CRGB*, ssize_t, const uint8_t> {
                matrix_string.getBuffer(),
                matrix_string.num_pixels,
                matrix_string.data_pin
            });
        }
        return buffers;
    }

};

#endif