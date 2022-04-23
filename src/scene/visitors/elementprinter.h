#ifndef __ELEMENT_PRINTER_H__
#define __ELEMENT_PRINTER_H__

#include "scene/visitors/elementvisitor.h"
#include <Arduino.h>
#include <etl/flat_map.h>

class ElementPrinter: ElementVisitor {
private:
    uint32_t cur_depth = 0;
    Print &out;

    void print_spacer() {
        for (uint i = 0; i < cur_depth; i++) {
            out.print("   ");
        }
    }

    void follow_children(Element* el);

public:
    ElementPrinter(Print &printer): out(printer) {}

    void visit(Scene* el);
    void visit(AdafruitGFXElement* el);
    void visit(BitmapElement* el);
};

#endif