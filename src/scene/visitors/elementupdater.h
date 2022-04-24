#ifndef __ELEMENT_UPDATER_H__
#define __ELEMENT_UPDATER_H__

#include "scene/visitors/elementvisitor.h"

class ElementUpdater: ElementVisitor {
private:
    void follow_children(Element* el);
    unsigned long last = 0;
    unsigned long delta = 0;

public:
    void setTimeDelta(unsigned long now);
    void visit(Scene* el);
    void visit(AdafruitGFXElement* el);
    void visit(BitmapElement* el);
    void visit(TargetFollowerElement* el);
    
};

#endif