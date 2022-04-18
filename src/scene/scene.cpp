#include "scene/scene.h"

Scene::Scene(std::initializer_list<Element*> children={})
    : Element("scene", children) 
    {}

void Scene::accept(ElementVisitor *visitor) {
    visitor->visit(this);
}