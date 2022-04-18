// #ifndef __Max7219_display_H__
// #define __Max7219_display_H__

// #include <Adafruit_GFX.h>
// #include <initializer_list>

// static const uint32_t max_matrix_strings = 2;

// class Max7219display : public Adafruit_GFX {

// public:
//     /**
//      * Do not allow default constructor
//      */
//     Max7219display() = delete;

//     /**
//      * Construct a WS2812Display with an initializer list of WS2812String objects
//      * 
//      * The display width of the WS2812Display will be equal to the sum of the width of the
//      * matrix objects in the strings
//      * 
//      * The display height will be equal to the max height of the matrices.
//      * 
//      * So essentially this will represent a bounding rectangle around the input matrices
//      * laid out in one long row
//      */
//     Max7219display (std::initializer_list<WS2812String*> strings): Adafruit_GFX {
//         std::accumulate(strings.begin(), strings.end(),
//                         (int16_t)0, [](int16_t a, const WS2812String* mat) {
//                             return a + mat->width();
//                         }),
//         std::accumulate(strings.begin(), strings.end(),
//                         (int16_t)0, [](int16_t a, const WS2812String* mat) {
//                             return std::max(a, mat->height());
//                         }),
//     } {
    
//     }

//     void drawPixel(int16_t x, int16_t y, uint16_t color) {
//         int32_t stringidx = lookupStringIndex(x);
        
//         if (stringidx < 0) {
//             return;
//         }

//         uint prev_boundary = 0;
//         if (stringidx > 0 ){
//             prev_boundary = string_boundaries[stringidx - 1];
//         } 

//         matrix_strings[stringidx]->drawPixel(x - prev_boundary, y, color);
//     }

//     void show() {
//         // FastLED.setBrightness(16);
//         unsigned long before = micros();
//         for (auto &matrixString: matrix_strings) {
//             matrixString->show(brightness);
//         }
//         // FastLED.show();
//         unsigned long after = micros();
//         Serial.print("FASTLED TOOK: ");
//         Serial.print(after - before);
//         Serial.println(" us");
//     }

//     void setBrightness(uint8_t scale) {
//         brightness = scale;
//     }

//     void clear() {
//         for (auto &matrixString: matrix_strings) {
//             matrixString->fillScreen(0);
//         }
//     }

// };

// #endif