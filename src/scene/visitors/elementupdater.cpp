#include "scene/visitors/elementupdater.h"
#include "scene/elements/element.h"
#include "scene/elements/bitmapelement.h"
#include "scene/elements/adagfxelement.h"
#include "scene/elements/targetfollowerelement.h"
#include "scene/scene.h"
#include "scene/visitors/elementdrawer.h"

void ElementUpdater::follow_children(Element* el) {
    for (Element* child: el->getChildren()) {
        child->accept(this);
    }
}

void ElementUpdater::setTimeDelta(unsigned long now) {
    delta = now - last;
    last = now;
}

void ElementUpdater::visit(Scene* el) {
    follow_children(el);
}

void ElementUpdater::visit(AdafruitGFXElement* el) {
    follow_children(el);
}

void ElementUpdater::visit(BitmapElement* el) {
    follow_children(el);
}

void ElementUpdater::visit(TargetFollowerElement* el) {
    // Update children
    follow_children(el);

    // // Render children to internal framebuffer
    // Scene tmpScene(el->getChildren());
    // ElementDrawer drawer(el->getTargetFramebuffer());
    // drawer.visit(&tmpScene);

    // // Calculate frame high and low per column
    // // el->getTargetFollowerLayer().recalculateTarget(el->getTargetFramebuffer());
    // el->recalculateLayerTargets(el->getTargetFramebuffer());
    // // Calculate delta factor
    // float deltaFactor = (float)delta / 80000;
    // deltaFactor = min(0.5f, max(deltaFactor, 0.1f));
    // // Serial.printf("updating with delta factor %.6f \n", deltaFactor);

    // // Move towards target
    // // el->getTargetFollowerLayer().update(deltaFactor);
    // el->updateLayers(deltaFactor);
    
    // // Serial.printf("Rendering (%d, %d) \n", el->getWidth(), el->getHeight());
    // el->renderToFramebuffer();

    // Also fade colors towards target
    // el->updateColors(deltaFactor);
}
