#ifndef __SCENE_MATRIX_ELEMENT_H__
#define __SCENE_MATRIX_ELEMENT_H__

#include <initializer_list>
#include <Adafruit_GFX.h>
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
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children = {}
    );

    void setBitmap(ProtoControl::IBitmap* bitmap) {
        this->bitmap = bitmap;
    }

    uint16_t getPixel(int16_t x, int16_t y);
    uint16_t getWidth();
    uint16_t getHeight();

    void accept(ElementVisitor *visitor);
};
#endif