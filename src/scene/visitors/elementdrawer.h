#ifndef __ELEMENT_DRAWER_H__
#define __ELEMENT_DRAWER_H__

#include "scene/visitors/elementvisitor.h"
// #include <Adafruit_GFX.h>
#include "ipixel.h"

class ElementDrawer: ElementVisitor {
private:
    void follow_children(Element* el);
    // Adafruit_GFX& display;
    IPixelReadWriteable& display;

public:
    ElementDrawer(IPixelReadWriteable &display)
    : display(display)
    {}

    void visit(Scene* el);
    void visit(AdafruitGFXElement* el);
    void visit(BitmapElement* el);
    void visit(TargetFollowerElement* el);
};

#endif