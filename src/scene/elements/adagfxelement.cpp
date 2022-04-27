#include "scene/elements/adagfxelement.h"

AdafruitGFXElement::AdafruitGFXElement(
        std::string name,
        uint16_t width,
        uint16_t height,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children
    )
    : Element(name, children) 
    , Adafruit_GFX(width, height)
    , draw_x(draw_x)
    , draw_y(draw_y)
    {
        framebuffer_size = width * height;
        framebuffer = new uint16_t[width * height];
    }

void AdafruitGFXElement::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Framebuffer is column major order
    framebuffer[x * HEIGHT + y] = color;
}

CRGB AdafruitGFXElement::getPixel(uint16_t x, uint16_t y) const {
    return framebuffer[x * HEIGHT + y];
}

uint16_t AdafruitGFXElement::getWidth() const {
    return width();
}

uint16_t AdafruitGFXElement::getHeight() const {
    return height();
}

void AdafruitGFXElement::accept(ElementVisitor *visitor) {
        visitor->visit(this);
    }