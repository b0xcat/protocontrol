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
    static uint32_t count;

public:
    template <typename... Params>
    Rainbow(Params&&... args)
    : T (std::forward<Params>(args)...)
    {}

    Rainbow(
        std::string name,
        uint32_t width,
        uint32_t height,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children = {}
    )
    : T (name, width, height, draw_x, draw_y, children)
    {}

    CRGB getPixel(uint16_t x, uint16_t y) const override {
        CHSV in(count / 100 + x * y, 255, 255);
        CRGB out;

        uint16_t cur_color = T::getPixel(x, y);

        if (cur_color == 0) {
            return cur_color;
        }

        count++;

        hsv2rgb_rainbow(in, out);

        return out;
    }
};

template <typename T>
uint32_t Rainbow<T>::count = 0;

#endif