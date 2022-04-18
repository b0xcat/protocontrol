#include <Arduino.h>
#include "scene/visitors/elementrgbbitmapsetter.h"

#include "scene/elements/element.h"
#include "scene/elements/bitmapelement.h"
#include "scene/scene.h"


void ElementRGBBitmapSetter::follow_children(Element* el) {
    for (Element* child: el->getChildren()) {
        child->accept(this);
    }
}

void ElementRGBBitmapSetter::visit(Scene* el) {
    follow_children(el);
}

void ElementRGBBitmapSetter::visit(BitmapElement* el) {
    // If the name matches, copy the bitmap to this element
    if (namemapping.contains(el->getName())) {
        el->drawRGBBitmap(
            0, 0,
            namemapping.at(el->getName()),
            el->width(), el->height()
        );
    }

    follow_children(el);
};
