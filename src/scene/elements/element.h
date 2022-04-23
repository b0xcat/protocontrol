#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <initializer_list>
#include <vector>
#include <string>
#include "scene/visitors/elementvisitor.h"

class Element {
    
protected:
    const std::string name;
    std::vector<Element*> _children;

public:
    Element(std::string name, std::initializer_list<Element*> children);

    std::vector<Element*> getChildren();

    std::string getName();

    virtual void accept(ElementVisitor *visitor) = 0;
};

// For modifier support
class IPixelReadable {
public:
    virtual uint16_t getPixel(int16_t x, int16_t y) = 0;
    virtual uint16_t getWidth() = 0;
    virtual uint16_t getHeight() = 0;
};

#endif