#ifndef __SCENE_MATRIX_TARGETFOLLOWER_ELEMENT_H__
#define __SCENE_MATRIX_TARGETFOLLOWER_ELEMENT_H__

// I AM SO SORRY

#include <initializer_list>
#include <Adafruit_GFX.h>
#include "crgbcanvas.h"
#include "scene/elements/element.h"
#include "scene/visitors/elementvisitor.h"
#include "bitmaps.h"
#include "scene/scene.h"
#include "colorconversion.h"
#include <etl/deque.h>

inline int round(float x) {
    return x >=0 ? (int)((x)+0.5) : (int)((x)-0.5);
};

static const float delta_e = 0.1;
class TargetFollowerLayer {
public:
    float* curHigh;
    float* curLow;
public:
    float* targetHigh;
    float* targetLow;
private:
    CRGB* curColors;
    CRGB* targetColors;
    uint16_t width;
    bool enabled;


    void interpolatecolumn(uint col, float target_high, float target_low, float deltaFactor)
    {
        float cur_high = this->curHigh[col];
        float cur_low = this->curLow[col];

        float delta_high = target_high - cur_high;
        this->curHigh[col] += deltaFactor * delta_high;
        
        float delta_low = target_low - cur_low;
        this->curLow[col] += deltaFactor * delta_low;
    }

public:
    TargetFollowerLayer(uint16_t width, bool enabled)
    : curHigh(new float[width])
    , curLow(new float[width])
    , targetHigh(new float[width])
    , targetLow(new float[width])
    , curColors(new CRGB[width])
    , targetColors(new CRGB[width])
    , width(width)
    , enabled(enabled)
    {
        for (uint32_t i = 0; i < width; i++) {
            curHigh[i] = -1.0;
            curLow[i] = -1.0;
            targetHigh[i] = -1.0;
            targetLow[i] = -1.0;
        }
    }

    void setEnabled(bool b) {
        enabled = b;
    }

    bool isEnabled() {
        return enabled;
    }

    void setColor(uint16_t idx, CRGB color) {
        targetColors[idx] = color;
    }

    
    void setLow(uint16_t idx, float value) {
        targetLow[idx] = value;
    }

    void setHigh(uint16_t idx, float value) {
        targetHigh[idx] = value;
    }
    
    float getLow(uint16_t idx) {
        return curLow[idx];
    }

    float getHigh(uint16_t idx) {
        return curHigh[idx];
    }

    bool getPixel(uint16_t x, uint16_t y) {
        if (x > width) {
            return false;
        }

        float low = curLow[x] - 0.5;
        float high = curHigh[x] + 0.5;

        float requested = (float) y;

        // Serial.printf("(%d, %d) Low: %f, High: %f, Requested: %f\n", x, y, low, high, requested);

        return requested >= low && requested <= high;
    }

    CRGB getColor(uint16_t col) {
        // Serial.printf("Getting color for column %d: %d", col, cur_color);
        return curColors[col];
    }

    void update(float deltaFactor) {
        if (!enabled) {
            return;
        }

        for (uint col = 0; col < width; col++)
        {
            float cur_target_high = this->targetHigh[col];
            float cur_target_low = this->targetLow[col];

            float cur_high = this->curHigh[col];
            float cur_low = this->curLow[col];

            // Case 1: Nothing to do:
            if (cur_high == cur_target_high && cur_low == cur_target_low)
            {
                continue;
            }
            // Case 2: appear from nothingness:
            else if (cur_high < 0 && cur_low < 0 && cur_target_high >= 0 && cur_target_low >= 0)
            {
                float midpoint = (cur_target_low + cur_target_high) * 0.5;
                this->curLow[col] = midpoint;
                this->curHigh[col] = midpoint;
            }
            // Case 3: disappear into nothingness:
            else if (cur_high >= 0 && cur_low >= 0 && cur_target_high < 0 && cur_target_low < 0)
            {
                float delta = abs(cur_high - cur_low);
                float midpoint = (cur_high + cur_low) * 0.5;
                // If close enough together, disappear:
                if (delta < delta_e)
                {
                    this->curHigh[col] = -1;
                    this->curLow[col] = -1;
                }
                else
                { // Else, move towards midpoint
                    interpolatecolumn(col, midpoint, midpoint, deltaFactor);
                }
            }
            // Case 4: move normally
            else if (cur_high >= 0 && cur_low >= 0 && cur_target_high >= 0 && cur_target_low >= 0)
            {
                interpolatecolumn(col, cur_target_high, cur_target_low, deltaFactor);
            }

            // Update colors
            // CRGB cur;
            // CRGB target;
            // convert565toCRGB(curColors[col], cur);
            // convert565toCRGB(targetColors[col], cur);

            curColors[col] = targetColors[col];
        }
    }
};

static const uint32_t n_layers = 8;

class TargetFollowerElement: public Element, public IPixelReadable {
private:
    std::unique_ptr<CRGBCanvas> target_framebuffer;
    std::unique_ptr<CRGBCanvas> current_framebuffer;
    std::array<TargetFollowerLayer*, n_layers> layers;

    // std::unique_ptr<TargetFollowerLayer> layer;

    uint16_t width;
    uint16_t height;

public:
    uint32_t draw_x;
    uint32_t draw_y;

    // TargetFollowerElement() = delete;

    TargetFollowerElement(
        std::string name,
        uint32_t width,
        uint32_t height,
        uint32_t draw_x,
        uint32_t draw_y,
        std::initializer_list<Element*> children = {}
    ) 
    : Element(name, children)
    , target_framebuffer(new CRGBCanvas(width, height))
    , current_framebuffer(new CRGBCanvas(width, height)) 
    // , layer(new TargetFollowerLayer(width, true))
    , width(width)
    , height(height)
    , draw_x(draw_x)
    , draw_y(draw_y)
    {
        for (auto &layer: layers) {
            layer = new TargetFollowerLayer(width, true);
        }
    }

    CRGBCanvas& getTargetFramebuffer() {
        return *target_framebuffer.get();
    }

    void renderToFramebuffer() {
        current_framebuffer->clear();
        uint layerno = 0;
        for (auto &layer: layers) {
            if (layer->isEnabled()) {

                for (uint32_t col = 0; col < width; col++) {
                    float _low = layer->getLow(col);
                    float _high = layer->getHigh(col);
                    
                    if (_low < 0 || _high < 0) {
                        continue;
                    }

                    // Serial.printf("layerno: %d, lowf: %f, highf: %f \n", layerno, _low, _high);

                    int low = layer->getLow(col) + 0.5f;
                    int high = layer->getHigh(col) + 0.5f;
                    
                    // Serial.printf("low: %d, high: %d \n", low, high);
                    current_framebuffer->drawFastVLine(
                        col, low, (high - low) + 1, layer->getColor(col)
                    );
                }

            }
            layerno++;
        }
    }

    TargetFollowerLayer& getTargetFollowerLayer(uint16_t n = 0) {
        // return *layer.get();
        return *layers[n];
    }

    void updateLayers(float deltaFactor) {
        for (auto &layer: layers) {
            layer->update(deltaFactor);
        }
    }

    void recalculateLayerTargets(CRGBCanvas &target) {
        uint32_t max_mask_layer_idx = 0;

        for (uint16_t col = 0; col < target.getWidth(); col++) {
            // TODO: fix this bullshit
            
            // uint16_t layer_idx = 0;
            // etl::deque<std::tuple<int, int, uint16_t>, n_layers> deq;

            // for (uint16_t row = 0; row < target.height(); row++) {

            //     uint16_t cur_pixel = target.getPixel(col, row);

            //     if (deq.empty()) {

            //         if (cur_pixel) {
            //             // find last pixel in this column with the same color
            //             uint16_t row_same_col = row;
            //             for (uint16_t row_inner = row; row_inner < target.height(); row_inner++) {
            //                 if (target.getPixel(col, row_inner) == cur_pixel) {
            //                     row_same_col = row_inner;
            //                 }
            //             }
            //             deq.push_back(std::make_tuple(row, row_same_col, cur_pixel));
            //         }

            //     } else {

            //         auto &prev_pixel = deque.back();
            //         uint16_t prev_color = std::get<1>(prev_pixel);

            //         if (cur_pixel != prev_color) {
            //             // find last pixel in this column with the same color
            //             uint16_t row_same_col = row;
            //             for (uint16_t row_inner = row; row_inner < target.height(); row_inner++) {
            //                 if (target.getPixel(col, row_inner) == cur_pixel) {
            //                     row_same_col = row_inner;
            //                 }
            //             }
            //             deq.push_back(std::make_tuple(row, row_same_col, cur_pixel));
            //         }
            //     }
                


            // }

            // Volatile as workaround for compiler bug, seems related to
            // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66358 ?
            volatile int16_t col_first = -1;
            volatile int16_t col_last = -1;

            uint32_t mask_layer_idx = 1;
            int32_t mask_first = -1;
            int32_t mask_last = -1;

            CRGB last_pixel = 0;
            CRGB cur_pixel = 0;
            CRGB col_color = 0;

            for (uint16_t row = 0; row < target.getHeight(); row++) {
                
                cur_pixel = target.getPixel(col, row);
                // Serial.printf("CUR PIXEL %d %d %d\n", cur_pixel.r, cur_pixel.g, cur_pixel.b);
                if (cur_pixel && col_first == -1) {
                    col_first = row;
                }

                if (cur_pixel) {
                    col_last = row;
                    col_color = cur_pixel;
                }


                if (col_first != -1 && mask_first == -1 && cur_pixel != last_pixel) {
                    mask_first = row;
                }
                if (col_first != -1 && mask_first != -1 && cur_pixel != last_pixel) {
                    mask_last = row;
                }
        

                if (mask_first != -1 && mask_last != -1) {
                    layers[mask_layer_idx]->setColor(col, cur_pixel);
                    layers[mask_layer_idx]->setHigh(col, mask_last);
                    layers[mask_layer_idx]->setLow(col, mask_first);

                    mask_first = -1;
                    mask_last = -1;
                    max_mask_layer_idx = max(mask_layer_idx, max_mask_layer_idx);
                    mask_layer_idx++;
                }

                last_pixel = cur_pixel;
            }
            // Serial.printf("col color (%d, %d, %d)\n", col_color.r, col_color.g, col_color.b);
            layers[0]->setColor(col, col_color);
            // layers[0]->targetLow[col] = col_first;
            // layers[0]->targetHigh[col] = col_last;
            layers[0]->setHigh(col, col_last);
            layers[0]->setLow(col, col_first);

            // Clear remaining layers
            for (uint32_t i = mask_layer_idx; i < n_layers; i++) {
                layers[i]->setHigh(col, -1);
                layers[i]->setLow(col, -1);
            }
 
        }
        // Disable unused layers
        for (uint32_t i = max_mask_layer_idx; i< n_layers; i++) {
            layers[i]->setEnabled(false);
        }
    }

    // Oh gawd please send help
    // void updateColors(float deltaFactor) {
    //     for (uint16_t x = 0; x < current_framebuffer->width(); x++) {
    //         for (uint16_t y = 0; y < current_framebuffer->height(); y++) {

    //             uint16_t cur_color = current_framebuffer->getPixel(x, y);
    //             uint16_t target_color = target_framebuffer->getPixel(x, y);

    //             CRGB cur;
    //             convert565toCRGB(cur_color, cur);
    //             // Serial.printf("Cur r %d, g %d, b %d \n", cur_r, cur_g, cur_b);

    //             CRGB target;
    //             convert565toCRGB(target_color, target);
    //             // Serial.printf("Target r %d, g %d, b %d \n", target_r, target_g, target_b);

    //             int16_t delta_r = target.r - cur.r;
    //             int16_t delta_g = target.g - cur.g;
    //             int16_t delta_b = target.b - cur.b;

    //             // Serial.printf("Delta r %d, g %d, b %d \n", delta_r, delta_g, delta_b);
    //             // uint8_t blendAmount = (uint8_t)(255.0 * deltaFactor);
    //             // CRGB newColor = blend(cur, target, blendAmount);

    //             CRGB newColor;
    //             newColor.r = (uint8_t)((float)cur.r + (float)delta_r * deltaFactor);
    //             newColor.g = (uint8_t)((float)cur.g + (float)delta_g * deltaFactor);
    //             newColor.b = (uint8_t)((float)cur.b + (float)delta_b * deltaFactor);
    //             // Serial.printf("New r %u, g %u, b %u \n", new_r, new_g, new_b);
    //             // Serial.println("lol");

    //             current_framebuffer->drawPixel(x, y, convertCRGBto565(newColor));
    //         }
    //     } 
    // }

    CRGB getPixel(uint16_t x, uint16_t y) const{
        // CRGB tmp;
        // uint16_t tmp_565 = current_framebuffer->getPixel(x, y);

        // convert565toCRGB(tmp_565, tmp);

        // // if (tmp_565) {
        // //     tmp = CRGB(255, 0, 0);
        // // }

        // Serial.printf("PIXEL(%d, %d) - (%d, %d, %d)\n", x, y, tmp.r, tmp.g, tmp.b);
        // return tmp;
        return current_framebuffer->getPixel(x, y);
        //TODO: wtf
        //return target_framebuffer->getPixel(x, y);
        // First check if it is masked or not
        // if (!layers[0]->getPixel(x, y)) {
        //     return 0;
        // }
        // Otherwise return the color
        // uint16_t col = current_framebuffer->getPixel(x, y);
        // if (col) {
        //     return col;
        // } else {
        // return layers[0]->getColor(x);
        // }
    }

    uint16_t getWidth() const {return width;}
    uint16_t getHeight() const {return height;}

    void accept(ElementVisitor *visitor) {
        visitor->visit(this);
    }
};
#endif