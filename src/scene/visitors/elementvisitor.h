#ifndef __ELEMENT_VISITOR_H__
#define __ELEMENT_VISITOR_H__

class Element;
class BitmapElement;
class Scene;

class ElementVisitor {
public:

    virtual void visit(BitmapElement* mme) = 0;

    virtual void visit(Scene* scene) = 0;

};
#endif