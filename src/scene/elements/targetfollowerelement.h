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
#include <etl/set.h>
#include <etl/vector.h>

extern bool debug;

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
    bool* colEnabled;


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
    , colEnabled(new bool[width])
    {
        for (uint32_t i = 0; i < width; i++) {
            curHigh[i] = -1.0;
            curLow[i] = -1.0;
            targetHigh[i] = -1.0;
            targetLow[i] = -1.0;
            curColors[i] = CRGB {0, 0, 0};
            targetColors[i] = CRGB {0, 0, 0};
            colEnabled[i] = false;
        }
    }

    void setEnabled(bool b) {
        enabled = b;
        if (!b) {
            for (uint32_t i = 0; i < width; i++) {
                colEnabled[i] = false;
            }
        }
    }

    void enableCol(uint32_t col_idx) {
        colEnabled[col_idx] = true;
    }

    bool isColEnabled(uint32_t col_idx) {
        return colEnabled[col_idx];
    }

    void clearTargets() {
        for (uint32_t i = 0; i < width; i++) {
            targetHigh[i] = -1.0;
            targetLow[i] = -1.0;
            targetColors[i] = CRGB {0, 0, 0};
        }
    }

    void clearCur() {
        for (uint32_t i = 0; i < width; i++) {
            curHigh[i] = -1.0;
            curLow[i] = -1.0;
            curColors[i] = CRGB {0, 0, 0};
        }
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
                //continue;
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

            // Blending still glitches out sometimes :(
            CRGB cur = curColors[col];
            CRGB target = targetColors[col];
            CRGB new_color = blend(cur, target, 32);
            curColors[col] = new_color;

            // curColors[col] = targetColors[col];
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

        // if (debug) {
        //     Serial.printf("Rendering %s to framebuffer\n", this->getName().c_str());    
        // }

        current_framebuffer->clear();
        uint layerno = 0;
        for (auto &layer: layers) {
            if (layer->isEnabled()) {

                for (uint32_t col = 0; col < width; col++) {
                    
                    if (!layer->isColEnabled(col)) {
                        continue;
                    }

                    float _low = layer->getLow(col);
                    float _high = layer->getHigh(col);
                    
                    if (_low < 0 || _high < 0) {
                        continue;
                    }

                    // if (debug) {
                    //     Serial.printf("layerno: %d, col: %d, lowf: %f, highf: %f \n", layerno, col, _low, _high);
                    // }

                    int low = layer->getLow(col) + 0.5f;
                    int high = layer->getHigh(col) - 0.5f;
                    CRGB color = layer->getColor(col);
                    
                    //Serial.printf("drawing low: %d, high: %d, color: (%d, %d, %d) \n", low, high, color.r, color.g, color.b);
                    current_framebuffer->drawFastVLine(
                        col, low, (high - low) + 1, color
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

private:
    void process_sublayer_column(const etl::vector<CRGB, n_layers> &column, 
                                    uint32_t col_idx,
                                    uint32_t start_i,
                                    uint32_t stop_i,
                                    uint32_t layer_idx) {
        CRGB base_color = column.at(start_i);
        CRGB layer_color = CRGB {0, 0, 0};
        int32_t min_i = start_i;
        int32_t max_i = stop_i;

        for (uint32_t i = start_i; i < stop_i; i++) {
            CRGB cur_color = column[i];

            if (min_i == start_i && cur_color != base_color) {
                min_i = i;
                layer_color = cur_color;
            }

            if (cur_color == layer_color) {
                max_i = i + 1;
            }
        }

        if (start_i == max_i - 1) {
            return;
        }

        layers[layer_idx]->setColor(col_idx, layer_color);
        layers[layer_idx]->setLow(col_idx, (float)min_i);
        layers[layer_idx]->setHigh(col_idx, (float)max_i);
        layers[layer_idx]->setEnabled(true);
        layers[layer_idx]->enableCol(col_idx);

        if (debug) {
            printf("Low: %f, High: %f Color: (%d, %d, %d)\n", (float)min_i, (float)max_i, layer_color.r, layer_color.g, layer_color.b);
        }

        process_sublayer_column(column, col_idx, max_i - 1, stop_i, layer_idx + 1);
    }

    void process_layer_column(const etl::vector<CRGB, n_layers> &column, const uint32_t col_idx) {
        volatile int32_t min_i = -1;
        volatile int32_t max_i = -1;
        CRGB layer_color = {0, 0, 0};
        bool should_recurse = false;
        etl::set<CRGB, 32> seen_colors;

        //Serial.printf("Processing column %d (size: %d)\n", col_idx, column.size());

        // Loop over the column and find the min and max non-zero indices
        // The base layer covers everything with one color
        for (uint32_t i = 0; i < column.size(); i++) {
            CRGB cur_color = column.at(i);
            
            if (debug) {
                Serial.printf("(%d, %d) Cur color: (%d, %d, %d)\n", col_idx, i, cur_color.r, cur_color.g, cur_color.b);
            }

            if (min_i == -1 && cur_color != CRGB {0, 0, 0}) {
                min_i = i;
                layer_color = cur_color;
            }

            if (cur_color != CRGB {0, 0, 0}) {
                max_i = i + 1;
            }

        }

        for (uint32_t i = min_i; i < max_i; i++) {
            CRGB cur_color = column.at(i);
            seen_colors.insert(cur_color);
        }

        if (seen_colors.size() > 1) {
            should_recurse = true;
        }

        //Serial.printf("min_i: %d, max_i: %d\n", min_i, max_i);
        
        // Empty column
        if (min_i == -1 && max_i == -1) {
            layers[0]->setColor(col_idx, CRGB {0, 0, 0});
            layers[0]->setLow(col_idx, -1.0f);
            layers[0]->setHigh(col_idx, -1.0f);
            layers[0]->setEnabled(true);
            layers[0]->enableCol(col_idx);
        } 
        // Non-empty column
        else {
            layers[0]->setColor(col_idx, layer_color);// CRGB{255, 0, 0});
            layers[0]->setLow(col_idx, (float)min_i);
            layers[0]->setHigh(col_idx, (float)max_i);
            layers[0]->setEnabled(true);
            layers[0]->enableCol(col_idx);

            // printf("Low: %f, High: %f Color: (%d, %d, %d)\n", (float)min_i, (float)max_i, layer_color.r, layer_color.g, layer_color.b);

            if (should_recurse) {
                Serial.println("RECURSING");
                process_sublayer_column(column, col_idx, min_i, max_i, 1);
            }
        }
    }

public:
    void recalculateLayerTargets(CRGBCanvas &target) {
        etl::vector<CRGB, n_layers> column;
        column.reserve(target.getHeight());

        if (debug) {
            Serial.print("NAME: ");        
            Serial.println(this->getName().c_str());
            target.print();
        }
        

        // Clear layers
        for (uint32_t i = 0; i< n_layers; i++) {
            layers[i]->setEnabled(false);
        }

        
        for (uint16_t col = 0; col < target.getWidth(); col++) {
            // column.fill(CRGB{0,0,0});
            column.clear();
            
            //Serial.print("Column: [");
            // Get the current column
            for (uint32_t row = 0; row < target.getHeight(); row++) {
                column.push_back(target.getPixel(col, row));
                //Serial.printf("(%d, %d, %d)", column[row].r, column[row].g, column[row].b);
                //Serial.print(" ");
            }
            //Serial.println("]");

            // Process the layers
            process_layer_column(column, col);
        }

    }

    CRGB getPixel(uint16_t x, uint16_t y) const{
        return current_framebuffer->getPixel(x, y);
    }

    uint16_t getWidth() const {return width;}
    uint16_t getHeight() const {return height;}

    void accept(ElementVisitor *visitor) {
        visitor->visit(this);
    }
};
#endif