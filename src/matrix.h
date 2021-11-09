#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <Arduino.h>
#include "layer.h"

class Painter;
#include "painter.h"

class Matrix {
    friend class MatrixManager;

    private:
    int id = -1;

    Layer **layers;
    uint n_layers;

    float animationspeed = 0.01;
    float prev_speed = 0.01;

    public:
    String name;
    
    // Matrix dimensions
    uint width;
    uint height;

    // Holds the "framebuffer"  for this matrix
    uint8_t *data;
    uint data_size;

    Matrix(String name_, uint w, uint h, uint n_layers_);

    uint getID();

    void setFrame(uint8_t *frame_data);

    void setPixel(uint row, uint col, bool value);

    bool getPixel(uint row, uint col);

    void blink();

    void clearBlink();

    void update(uint32_t deltatime);

    void paint(Painter &painter);
};
#endif