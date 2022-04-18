#ifndef __SCENE_MATRIX_ELEMENT_H__
#define __SCENE_MATRIX_ELEMENT_H__

#include <initializer_list>
#include <Adafruit_GFX.h>
#include "scene/elements/element.h"
#include "scene/visitors/elementvisitor.h"


class BitmapElement: public Element, public Adafruit_GFX {
private:
    uint16_t* framebuffer;
    ssize_t framebuffer_size;

public:
    uint32_t draw_x;
    uint32_t draw_y;

    BitmapElement() = delete;

    BitmapElement(
        std::string name,
        uint16_t width,
        uint16_t height,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children = {}
    );

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    uint16_t getPixel(int16_t x, int16_t y);

    void accept(ElementVisitor *visitor);

    ~BitmapElement() {
        delete[] framebuffer;
    }
};
#endif