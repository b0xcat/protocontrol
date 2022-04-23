#ifndef __SCENE_MODIFIER_RAINBOW_H__
#define __SCENE_MODIFIER_RAINBOW_H__

#include "scene/elements/element.h"
#include "colorconversion.h"
#include <FastLED.h>

// Man, wish I could use concepts rn
// T should be an IPixelReadable I guess?
template <typename T> 
class Rainbow: public T{
private:
    uint32_t count;

public:
    template <typename... Params>
    Rainbow(Params&&... args)
    : T (std::forward<Params>(args)...)
    {}

    uint16_t getPixel(int16_t x, int16_t y) {
        CHSV in(count / 33 + x * y, 255, 255);
        CRGB out;

        uint16_t cur_color = T::getPixel(x, y);

        if (cur_color == 0) {
            return cur_color;
        }

        count++;

        hsv2rgb_rainbow(in, out);

        return fromRGB(out.r, out.g, out.b);
    }
};

#endif