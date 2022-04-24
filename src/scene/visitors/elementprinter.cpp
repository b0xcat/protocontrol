#include <Arduino.h>
#include "scene/visitors/elementprinter.h"

#include "scene/elements/element.h"
#include "scene/elements/bitmapelement.h"
#include "scene/elements/adagfxelement.h"
#include "scene/elements/targetfollowerelement.h"
#include "scene/scene.h"


void ElementPrinter::follow_children(Element* el) {
    cur_depth += 1;
    for (Element* child: el->getChildren()) {
        child->accept(this);
    }
    cur_depth -= 1;
}

void ElementPrinter::visit(Scene* el) {
    print_spacer();

    out.print(el->getName().c_str());
    out.println(":");

    follow_children(el);
}

void ElementPrinter::visit(AdafruitGFXElement* el) {
    print_spacer();

    out.print(el->getName().c_str());
    out.print("[");
    out.print(el->width());
    out.print(", ");
    out.print(el->height());
    out.println("]");
    
    follow_children(el);
};

void ElementPrinter::visit(BitmapElement* el) {
    print_spacer();

    out.print(el->getName().c_str());
    out.println();
    // out.print("[");
    // out.print(el->width());
    // out.print(", ");
    // out.print(el->height());
    // out.println("]");
    
    follow_children(el);
};

void ElementPrinter::visit(TargetFollowerElement* el) {
    print_spacer();

    out.print(el->getName().c_str());
    out.println();

    follow_children(el);
};