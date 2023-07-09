#ifndef __SCENE_MODIFIER_COLOROVERRIDE_H__
#define __SCENE_MODIFIER_COLOROVERRIDE_H__

#include "scene/elements/element.h"
#include "colorconversion.h"
#include <FastLED.h>

extern CRGB overrideColor;

// Man, wish I could use concepts rn
// T should be an IPixelReadable I guess?
template <typename T> 
class ColorOverride: public T{
public:
    template <typename... Params>
    ColorOverride(Params&&... args)
    : T (std::forward<Params>(args)...)
    {}

    ColorOverride(
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
        CRGB cur_color = T::getPixel(x, y);

        if (cur_color == CRGB{0, 0, 0}) {
            return cur_color;
        }

        return overrideColor;
    }
};

#endif