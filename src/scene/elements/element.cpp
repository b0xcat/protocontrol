#include "scene/elements/element.h"

std::vector<Element*> Element::getChildren() {
    return _children;
}


std::string Element::getName() {
    return name;
}