#include "painter.h"

Painter& Painter::setPos(Matrix* m, uint x, uint y) {
    drawpos[m->getID()] = {true, x, y};
    return *this;
}

Painter& Painter::setPos(String name, uint x, uint y) {
    namemap[namemapcount] = {name, {true, x, y}};
    namemapcount++;
    return *this;
}

DrawPos Painter::getPos(Matrix* m) {
    int id = m->getID();
    if (id != -1) {
        if (drawpos[id].isSet) {
            return drawpos[id];
        } else {
            // Serial.print("getting pos for ");
            // Serial.println(m->name);
            DrawPos tmp = getPos(m->name);
            // Serial.println("Setting pos");
            setPos(m, tmp.x, tmp.y);
            // Serial.println("Pos set");
            return tmp;
        }
    } else {
        return {false, 0, 0};
    }
}

DrawPos Painter::getPos(String name) {
    // Serial.print("Lookup by name for: ");
    // Serial.println(name);
    for (uint i = 0; i < namemapcount; i++) {
        // Serial.println(i);
        // Serial.print("Namemap: ");
        if (namemap[i].name == name) {
            // Serial.print(namemap[i].name);
            // Serial.print(namemap[i].pos.x);
            // Serial.print(",");
            // Serial.println(namemap[i].pos.y);
            return namemap[i].pos;
        }
    }
    //Serial.println("OH NO");
    return {false, 0, 0};
}



void Painter::drawPixel(Matrix* m, uint x, uint y, uint16_t color) {
    DrawPos pos = getPos(m);
    drawPixel(x + pos.x, y + pos.y, color);
} 

void Painter::drawVLine(Matrix* m, uint x, uint y, uint len, uint16_t color){
    DrawPos pos = getPos(m);
    drawVLine(x + pos.x, y + pos.y, len, color);
}