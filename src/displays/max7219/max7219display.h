#ifndef __Max7219_display_H__
#define __Max7219_display_H__

#include <initializer_list>
#include "ipixel.h"
#include <Adafruit_GFX.h>
#include "Max72xxPanel.h"

static const uint32_t max_matrix_strings = 2;

class Max7219display : public IPixelReadWriteable
{

private:
    Max72xxPanel &panel;

public:
    /**
     * Do not allow default constructor
     */
    Max7219display() = delete;

    Max7219display(Max72xxPanel &panel) : panel{panel} {}

    void setPixel(uint16_t x, uint16_t y, CRGB color) override
    {   
        panel.drawPixel(x, y, color ? 1 : 0);
    }

    CRGB getPixel(uint16_t x, uint16_t y) const override
    {
        return CRGB(0, 0, 0);
    }

    Max72xxPanel &getPanel()
    {
        return panel;
    }

    void show()
    {
        this->panel.write();
    }

    void setBrightness(uint8_t scale)
    {
        this->panel.setIntensity(max(0, min(scale >> 4, 15)));
    }

    void clear() override
    {
        this->panel.fillScreen(0);
    }

    uint16_t getWidth() const override { return 0; }
    uint16_t getHeight() const override { return 0; }
};

#endif