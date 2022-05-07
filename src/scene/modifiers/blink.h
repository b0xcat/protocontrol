#ifndef __SCENE_MODIFIER_BLINK_H__
#define __SCENE_MODIFIER_BLINK_H__

#include "scene/elements/element.h"

#include "colorconversion.h"
#include <FastLED.h>

template <typename T>
class Controller {
private:
    T var;
public:
    Controller(T var)
    : var(var)
    {}

    virtual T get() {
        return var;
    }

    virtual void set(T newvar) {
        var = newvar;
    }
};

class BlinkController: Controller<bool> {
private:
    uint32_t blink_duration;
    uint32_t blink_frequency;
    uint32_t blink_randomness;

    unsigned long prev {};
    unsigned long accumulator {};
    uint32_t cycle_duration;

public:
    BlinkController(bool var,
                    uint32_t blink_duration,
                    uint32_t blink_frequency,
                    uint32_t blink_randomness)
                    : Controller<bool>(var)
                    , blink_duration(blink_duration)
                    , blink_frequency(blink_frequency)
                    , blink_randomness(blink_randomness)
                    , cycle_duration(blink_frequency)
                    {}
    
    bool get() override {

        unsigned long now = millis();
        unsigned long delta = now - prev;
        accumulator += delta;
        prev = now;

        if (accumulator > cycle_duration) {
            accumulator = 0;
            cycle_duration = blink_frequency + random(-blink_randomness, blink_randomness);
        }

        return accumulator < blink_duration;
    }

};

// Wrap this around a bitmapelement to automatically generate a
// "closed" variant of it. e.g. a closed eye (for blinking)
template <typename T> 
class Blink: public T{
private: 
    std::unique_ptr<CRGB[]> crgb_buf;
    uint32_t w {};
    uint32_t h {};
    BlinkController& controller;

public:
    template <typename... Params>
    Blink(BlinkController& controller, Params&&... args)
    : T (std::forward<Params>(args)...)
    , controller(controller)
    {}

    CRGB getPixel(uint16_t x, uint16_t y) const override {
        if (controller.get()) {
            if (crgb_buf.get()) {
            return crgb_buf[x * h + y];
        }
            return CRGB {0, 0, 0};
        } else {
            return T::getPixel(x, y);
        }
    }

    void setPixel(uint16_t x, uint16_t y, CRGB color) {
        crgb_buf[x * h + y] = color;
    }

    void setBitmap(ProtoControl::IBitmap* bitmap) override {
        T::setBitmap(bitmap);
        w = T::getWidth();
        h = T::getHeight();

        // Serial.printf("Bitmap set. w: %d, h: %d\n", w, h);

        crgb_buf = std::make_unique<CRGB[]>(w * h);

        calculateBlinkBitmap();
    }

private:
    void calculateBlinkBitmap() {
        uint32_t global_max = 0;
        std::vector<CRGB> cols_used;
        cols_used.reserve(w);

        for (uint32_t col = 0; col < w; col++) {
            cols_used[col] = CRGB {0, 0, 0};
        }

        for (uint32_t col = 0; col < w; col++) {
            for (uint32_t row = 0; row < h; row++) {
                CRGB cur_pixel = T::getPixel(col, row);
                if (cur_pixel != CRGB{0,0,0}) {
                    if (row > global_max) {
                        global_max = row;
                    }
                    cols_used[col] = cur_pixel;
                }
            }
        }

        for (uint32_t col = 0; col < w; col++) {
            if (cols_used[col] != CRGB{0, 0, 0}) {
                setPixel(col, global_max, cols_used[col]);
            }
        }
    }
};

#endif