#ifndef __MATRIXMANAGER_H__
#define __MATRIXMANAGER_H__

#include <Arduino.h>
#include "matrix.h"
#include "painter.h"

#define MAX_MATRICES 64

class MatrixManager {
    private:
    Matrix *matrices[MAX_MATRICES];
    uint n_matrices = 0;

    bool blinks[MAX_MATRICES];

    public:
    uint8_t *data;
    uint data_size;

    MatrixManager() {
        for (uint i = 0; i < MAX_MATRICES; i++) {
            blinks[i] = false;
        }
    }

    void add(Matrix *m) {
        Serial.print("Adding matrix ");
        Serial.println(m->name);
        matrices[n_matrices] = m;
        m->id = n_matrices;
        n_matrices++;
    }

    Matrix* get(String name) {
        for (uint i = 0; i < n_matrices; i++) {
            if (matrices[i]->name == name) {
                return matrices[i];
            }
        }
        
        Serial.print("ERR: matrix ");
        Serial.print(name);
        Serial.println(" not found!");

        return nullptr;
    }

    void init() {
        // Calculate total necessary framebuffer size:
        for (uint i = 0; i < n_matrices; i++) {
            data_size += matrices[i]->data_size;
        }

        // Init memory
        data = new uint8_t[data_size];
        for (uint i = 0; i < data_size; i++) {
            data[i] = 0;
        }

        // Tell the matrices where to find their memory
        uint counter = 0;
        for (uint i = 0; i < n_matrices; i++) {
            matrices[i]->data = data + counter;
            counter += matrices[i]->data_size;
        }
    }

    void setFrame(uint8_t *data_) {
        // Copy the data if it's different
        if (data != data_) {
            for (uint i = 0; i < data_size; i++) {
                data[i] = data_[i];
            }    
        }
        
        // Tell the matrices to update their internal rep
        for (uint i = 0; i < n_matrices; i++) {
            matrices[i]->setFrame(matrices[i]->data);
        }        
    }

    void setMatrixBlink(String name, bool shouldblink) {
        Serial.print("Setting blink on matrix ");
        for (uint i = 0; i < n_matrices; i++) {
            if (matrices[i]->name == name) {
                Serial.println(matrices[i]->name);
                blinks[i] = shouldblink;
            }
        }
    }

    void blink() {
        for (uint i = 0; i < n_matrices; i++) {
            if (blinks[i]) {
                matrices[i]->blink();
            }
        }
    }

    void clearBlink() {
        for (uint i = 0; i < n_matrices; i++) {
            if (blinks[i]) {
                matrices[i]->clearBlink();
            }
        }
    }

    void update(uint32_t deltatime)
    {
        for (uint i = 0; i < n_matrices; i++)
        {
            matrices[i]->update(deltatime);
        }
    }

    void paint(Painter &painter)
    {
        painter.clear();
        for (uint i = 0; i < n_matrices; i++)
        {
            matrices[i]->paint(painter);
        }
        painter.show();
    }
};

#endif