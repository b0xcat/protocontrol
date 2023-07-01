#include <Arduino.h>
#include "scene/visitors/elementrgbbitmapsetter.h"

#include "scene/elements/element.h"
#include "scene/elements/bitmapelement.h"
#include "scene/elements/adagfxelement.h"
#include "scene/elements/targetfollowerelement.h"
#include "scene/scene.h"
#include "bitmaps.h"


inline uint16_t crgbToRGB565(CRGB color) {
  return ((color.r & 0xF8) << 8) | ((color.g & 0xFC) << 3) | ((color.b & 0xF8) >> 3);
}


void ElementRGBBitmapSetter::follow_children(Element* el) {
    for (Element* child: el->getChildren()) {
        child->accept(this);
    }
}

void ElementRGBBitmapSetter::visit(Scene* el) {
    follow_children(el);
}

void ElementRGBBitmapSetter::visit(AdafruitGFXElement* el) {
    // If the name matches, copy the bitmap to this element
    if (namemapping.contains(el->getName())) {
        ProtoControl::IBitmap* cur_bitmap = namemapping.at(el->getName());

        for (uint16_t x = 0; x < el->width(); x++) {
            for (uint16_t y = 0; y < el->height(); y++) {
                el->drawPixel(x, y, crgbToRGB565(cur_bitmap->getPixel(x, y)));
            }
        }
    }

    follow_children(el);
};

void ElementRGBBitmapSetter::visit(BitmapElement* el) {
    // If the name matches, set the pointer to this bitmap
    if (namemapping.contains(el->getName())) {
        ProtoControl::IBitmap* cur_bitmap = namemapping.at(el->getName());
        el->setBitmap(cur_bitmap);
    }

    follow_children(el);
}

void ElementRGBBitmapSetter::visit(TargetFollowerElement* el) {
    follow_children(el);
}