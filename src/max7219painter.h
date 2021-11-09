#ifndef __MAX_PAINTER_H__
#define __MAX_PAINTER_H__

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "painter.h"
#define NUMBER_OF_DEVICES 14 //number of led matrix connect in series
#ifndef DELTA_E
#define DELTA_E 0.5
#endif

class Max7219painter : public Painter {
private:
    Max72xxPanel& face;

public:
    Max7219painter(Max72xxPanel& face): face(face) {

        // Face config
        for (uint i = 0; i < NUMBER_OF_DEVICES; i++){
            face.setRotation(i, 1);
        }
        face.setRotation(13, 3);
        face.setRotation(12, 3);

        face.setPosition(13, 12, 0);
        face.setPosition(12, 13, 0);

        // Set brightness
        face.setIntensity(1);

        // Write settings
        face.write();

        // Startup blink
        for (uint i = 0; i < 3; i++) {
            face.fillScreen(1);
            face.write();
            delay(100);
            face.fillScreen(0);
            face.write();
            delay(100);
        }
    }

    void drawPixel(uint x, uint y, uint16_t color) {
        face.drawPixel(x, y, color);
    }

    void drawVLine(uint x, uint y, uint len, uint16_t color) {
        face.drawFastVLine(x, y, len, color);
    }

    void show() {
        face.write();
    }

    void clear() {
        face.fillScreen(0);
    }

    void showmsg(String msg) {
        face.print(msg);
    }

};

#endif