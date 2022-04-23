#ifndef __SCENE_MODIFIER_MIRROR_H__
#define __SCENE_MODIFIER_MIRROR_H__

#include "scene/elements/element.h"

// Man, wish I could use concepts rn
// T should be an IPixelReadable I guess?
template <typename T> 
class MirrorHorizontal: public T{
public:
    template <typename... Params>
    MirrorHorizontal(Params&&... args)
    : T (std::forward<Params>(args)...)
    {}

    uint16_t getPixel(int16_t x, int16_t y) {
        return T::getPixel(T::getWidth() - 1 - x, y);
    }
};

#endif