#include "matrix.h"

Matrix::Matrix(String name_, uint w, uint h, uint n_layers_) {
    width = w;
    height = h;

    name = name_;

    // Create the buffer holding the frame
    // Actually no, just calc the size here
    data_size = ceil((w * h) / 8.0f);
    //data = new uint8_t[data_size];

    // Create and initialize the layers
    n_layers = n_layers_;
    layers = new Layer*[n_layers];
    for(uint i = 0; i < n_layers; i++) {
        layers[i] = new Layer(width);
    }

    // Assign color to layers, first layer has color, other layers are mask
    layers[0]->set_target_color(1);
    for (uint i = 1; i < n_layers; i++)
    {
        layers[i]->set_target_color(0);
    }
}

uint Matrix::getID() {
    return id;
}

void Matrix::setFrame(uint8_t *frame_data){
    //Serial.print("Setting frame on ");
    //Serial.println(this->name);

    // Create internal representation
    for (uint col = 0; col < width; col++) {

        // Find the first and last pixel on in this column
        int first = -1;
        int last = -1;

        // Keep track of mask stuff too
        uint mask_layer_count = 1;
        int mask_first = -1;
        int mask_last = -1;

        for (uint row = 0; row < height; row++) {

            //bool cur_pixel_on = getbit(fb, col_begin * 8 + (col - col_begin) + col_len * row);
            //Serial.println("Getting pixel");
            bool cur_pixel_on = getPixel(row, col);
            // Serial.print("Pixel: ");
            // Serial.print(row);
            // Serial.print(", ");
            // Serial.print(col);
            // Serial.print(", ");
            // Serial.println(cur_pixel_on);
            
            // Base layer checks
            if (cur_pixel_on && first == -1) {
                first = row;
            }

            if (cur_pixel_on) {
                last = row;
            }

            // Mask layer checks
            if (first != -1 && mask_first == -1 && !cur_pixel_on) {
                mask_first = row;
            }
            if (first != -1 && mask_first != -1 && cur_pixel_on) {
                mask_last = row - 1;
            }
            if (mask_first != -1 && mask_last != -1) {
                if (mask_layer_count > n_layers) {
                    Serial.println("ERROR too many layers");
                    return;
                }

                layers[mask_layer_count]->set_target_low(mask_first, col);
                layers[mask_layer_count]->set_target_high(mask_last, col);
                
                mask_first = -1;
                mask_last = -1;

                mask_layer_count++;
            }
        }

        layers[0]->set_target_low(first, col);
        layers[0]->set_target_high(last, col);

        // also clear unused layers
        for (size_t i = mask_layer_count; i < n_layers; i++) {
            layers[i]->set_target_low(-1, col);
            layers[i]->set_target_high(-1, col);
        }
    }
}

void Matrix::setPixel(uint row, uint col, bool value) {
    uint pos = col + width * row;

    int byteidx = pos / (sizeof(uint8_t) * 8);
    int bitidx = pos % (sizeof(uint8_t) * 8);

    uint8_t tmp = data[byteidx];
    if (value == 0) {
        data[byteidx] = tmp & (0xFF & ~(0x01 << (7 - bitidx)));
    } else {
        data[byteidx] = tmp | (0x01 << (7 - bitidx));
    }
}

bool Matrix::getPixel(uint row, uint col) {
    // if (row > height || col > width) {
    //     Serial.println("Out of bounds pixel access!");
    // }
    
    uint pos = col + width * row;
    // if (pos > data_size * 8) {
    //     Serial.println("Out of bounds pos!");
    // }

    int byteidx = pos / (sizeof(uint8_t) * 8);
    int bitidx = pos % (sizeof(uint8_t) * 8);

    uint8_t tmp = data[byteidx];

    int bit = (tmp >> bitidx) & 1;

    return (bool)bit;
}

void Matrix::blink() {
    prev_speed = animationspeed;
    animationspeed = 0.04;

    // find lowest row used, and which columns are used
    uint lowest_row = 0;
    bool cols_used[width];

    for (uint col = 0; col < width; col++) {
        
        cols_used[col] = false;

        for (uint row = 0; row < height; row++) {
            //bool cur_pixel_on = getbit(fb, col_begin * 8 + (col - col_begin) + col_len * row); what?
            bool cur_pixel_on = getPixel(row, col);
            if (cur_pixel_on) {
                cols_used[col] = true;
                if (row > lowest_row) {
                    lowest_row = row;
                }
            }
        }
    }
    for (uint col = 0; col < width; col++) {
        if (cols_used[col]) {
            layers[0]->set_target_low(lowest_row, col);
            layers[0]->set_target_high(lowest_row + 1, col);
        } else {
            layers[0]->set_target_low(-1, col);
            layers[0]->set_target_high(-1, col);
        }
        // also clear unused layers
        for (size_t i = 1; i < n_layers; i++) {
            layers[i]->set_target_low(-1, col);
            layers[i]->set_target_high(-1, col);
        }
    }
}

void Matrix::clearBlink() {
    animationspeed = prev_speed;
    setFrame(data);
}

void Matrix::update(uint32_t deltatime)
{
    for (uint i = 0; i < n_layers; i++)
    {
        layers[i]->update(deltatime, animationspeed);
    }
}

void Matrix::paint(Painter &painter)
{
    for (uint i = 0; i < n_layers; i++)
    {
        // TODO: this is kinda inefficient, potentially quite a bit of overdraw
        // Maybe first determine what pixels should be touched?
        for (uint col = 0; col < width; col++) {
            int low = layers[i]->getLow(col);
            int high = layers[i]->getHigh(col);

            if (low != -1 && high != -1) {
                uint16_t color = layers[i]->getColor(col);

                painter.drawVLine(
                    this,
                    col,
                    low,
                    high - low,
                    color
                );
            }
        }
    }
}