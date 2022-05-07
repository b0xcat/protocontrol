#ifndef __SCENE_MATRIX_ELEMENT_H__
#define __SCENE_MATRIX_ELEMENT_H__

#include <initializer_list>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include "scene/elements/element.h"
#include "scene/visitors/elementvisitor.h"
#include "bitmaps.h"

class BitmapElement: public Element, public IPixelReadable {
private:
    ProtoControl::IBitmap* bitmap;

public:
    uint32_t draw_x;
    uint32_t draw_y;

    BitmapElement() = delete;

    BitmapElement(
        std::string name,
        uint32_t draw_x = 0,
        uint32_t draw_y = 0,
        std::initializer_list<Element*> children = {}
    );

    virtual void setBitmap(ProtoControl::IBitmap* bitmap) {
        this->bitmap = bitmap;
    }

    CRGB getPixel(uint16_t x, uint16_t y) const;
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    void accept(ElementVisitor *visitor);
};
#endif