#ifndef __PAINTER_H__
#define __PAINTER_H__

#include "Arduino.h"
class Matrix;
#include "matrix.h"

#define MAX_ELEMS 64

struct DrawPos {
    DrawPos() : isSet(false), x(0), y(0) {}
    DrawPos(bool set, uint x_, uint y_): isSet(set), x(x_), y(y_) {}
    DrawPos(uint x_, uint y_): isSet(true), x(x_), y(y_) {}
    bool isSet;
    uint x;
    uint y;
};

struct DrawPosNameMapping {
    String name;
    DrawPos pos;
};

class Painter
{
private: 
    DrawPos drawpos[MAX_ELEMS];
    DrawPosNameMapping namemap[MAX_ELEMS];
    uint namemapcount = 0;

public:
    Painter& setPos(Matrix* m, uint x, uint y);
    
    Painter& setPos(String name, uint x, uint y);

    DrawPos getPos(Matrix* m);

    DrawPos getPos(String name);
    
    template <size_t array_size>
    void setNameMapping(const DrawPosNameMapping (&namemap)[array_size])
    {
        for (DrawPosNameMapping namemapping : namemap) {
            setPos(namemapping.name, namemapping.pos.x, namemapping.pos.y);
        }
    }

    void drawPixel(Matrix* m, uint x, uint y, uint16_t color);

    void drawVLine(Matrix* m, uint x, uint y, uint len, uint16_t color);

    virtual void drawPixel(uint x, uint y, uint16_t color) = 0;

    virtual void drawVLine(uint x, uint y, uint len, uint16_t color) = 0;

    virtual void show() = 0;

    virtual void clear() = 0;

    virtual void showmsg(String msg) = 0;
};

#endif