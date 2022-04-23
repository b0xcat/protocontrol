#include "scene/visitors/elementdrawer.h"
#include "scene/elements/element.h"
#include "scene/elements/bitmapelement.h"
#include "scene/elements/adagfxelement.h"
#include "scene/scene.h"


void ElementDrawer::follow_children(Element* el) {
    for (Element* child: el->getChildren()) {
        child->accept(this);
    }
}

void ElementDrawer::visit(Scene* el) {
    follow_children(el);
}

void ElementDrawer::visit(AdafruitGFXElement* el) {

    const uint32_t draw_x = el->draw_x;
    const uint32_t draw_y = el->draw_y;

    for (uint y = 0; y < el->height(); y++) {
        for (uint x = 0; x < el->width(); x++ ) {
            display.drawPixel(draw_x + x, draw_y + y, el->getPixel(x, y));
        }
    }
    
    follow_children(el);
}

void ElementDrawer::visit(BitmapElement* el) {
    
}
