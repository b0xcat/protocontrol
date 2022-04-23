#ifndef __ELEMENT_VISITOR_H__
#define __ELEMENT_VISITOR_H__

class Element;
class AdafruitGFXElement;
class BitmapElement;
class Scene;

class ElementVisitor {
public:

    virtual void visit(AdafruitGFXElement* el) = 0;

    virtual void visit(BitmapElement* el) = 0;

    virtual void visit(Scene* scene) = 0;

};
#endif