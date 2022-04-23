#ifndef __ELEMENT_BITMAP_SETTER_H__
#define __ELEMENT_BITMAP_SETTER_H__

#include "scene/visitors/elementvisitor.h"
#include "bitmaps.h"
#include <Arduino.h>
#include <etl/flat_map.h>

// To allocate flatmap memory beforehand
static const uint32_t map_size = 64;

class ElementRGBBitmapSetter: ElementVisitor {
private:
    etl::flat_map<std::string, ProtoControl::IBitmap*, map_size> namemapping;
    void follow_children(Element* el);

public:
    ElementRGBBitmapSetter() {}

    ElementRGBBitmapSetter& add(std::string name, ProtoControl::IBitmap* bitmap) {
        namemapping[name] = bitmap;
        return *this;
    }

    void visit(Scene* el);
    void visit(AdafruitGFXElement* el);
    void visit(BitmapElement* el);
};

#endif