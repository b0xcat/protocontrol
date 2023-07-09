#include "scene/scene.h"

Scene::Scene(std::initializer_list<Element*> children={})
    : Element("scene", children) 
    {}

Scene::Scene(std::vector<Element*> children)
: Element("scene", children.begin(), children.end())
{}

void Scene::accept(ElementVisitor *visitor) {
    visitor->visit(this);
}