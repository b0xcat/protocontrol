#include "scene/elements/bitmapelement.h"

BitmapElement::BitmapElement(
        std::string name,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children
    )
    : Element(name, children) 
    , draw_x(draw_x)
    , draw_y(draw_y)
    {}

CRGB BitmapElement::getPixel(uint16_t x, uint16_t y) const {
    return bitmap->getPixel(x, y);
}

uint16_t BitmapElement::getWidth() const {
    return bitmap->getWidth();
}

uint16_t BitmapElement::getHeight() const {
    return bitmap->getHeight();
}

void BitmapElement::accept(ElementVisitor *visitor) {
        visitor->visit(this);
    }