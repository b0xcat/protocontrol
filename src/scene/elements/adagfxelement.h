#ifndef __SCENE_ADAGFX_MATRIX_ELEMENT_H__
#define __SCENE_ADAGFX_MATRIX_ELEMENT_H__

#include <initializer_list>
#include <Adafruit_GFX.h>
#include "scene/elements/element.h"
#include "scene/visitors/elementvisitor.h"


class AdafruitGFXElement: public Element, public Adafruit_GFX, public IPixelReadable {
private:
    uint16_t* framebuffer;
    ssize_t framebuffer_size;

public:
    uint32_t draw_x;
    uint32_t draw_y;

    AdafruitGFXElement() = delete;

    AdafruitGFXElement(
        std::string name,
        uint16_t width,
        uint16_t height,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children = {}
    );

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    CRGB getPixel(uint16_t x, uint16_t y) const;
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    
    void accept(ElementVisitor *visitor);

    ~AdafruitGFXElement() {
        delete[] framebuffer;
    }
};
#endif