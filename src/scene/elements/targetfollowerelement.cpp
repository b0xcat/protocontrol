// #include "scene/elements/bitmapelement.h"

// BitmapElement::BitmapElement(
//         std::string name,
//         uint32_t draw_x,
//         uint32_t draw_y,
//         std::initializer_list<Element*> children
//     )
//     : Element(name, children) 
//     , draw_x(draw_x)
//     , draw_y(draw_y)
//     {}

// uint16_t BitmapElement::getPixel(int16_t x, int16_t y) {
//     return bitmap->getPixel(x, y);
// }

// uint16_t BitmapElement::getWidth() {
//     return bitmap->getWidth();
// }

// uint16_t BitmapElement::getHeight() {
//     return bitmap->getHeight();
// }

// void BitmapElement::accept(ElementVisitor *visitor) {
//         visitor->visit(this);
//     }