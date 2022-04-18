#include "scene/elements/element.h"

Element::Element(std::string name, std::initializer_list<Element*> children)
: name(name) 
{
    // Append the initializer list to the children list
    _children.insert(_children.end(), children.begin(), children.end());
}

std::vector<Element*> Element::getChildren() {
    return _children;
}


std::string Element::getName() {
    return name;
}