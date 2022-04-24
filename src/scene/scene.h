#ifndef __SCENE_H__
#define __SCENE_H__

#include <initializer_list>
#include "scene/elements/element.h"


class Scene : public Element {
public:
    Scene(std::initializer_list<Element*> children);
    Scene(std::vector<Element*> children);

    void accept(ElementVisitor *visitor) override;
};
#endif