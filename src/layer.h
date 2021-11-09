#ifndef __LAYER_H__
#define __LAYER_H__

#ifndef DELTA_E
#define DELTA_E 0.5
#endif

#include "painter.h"

class Layer
{
private:
    // uint16_t cur_color[N_COLS];
    // uint16_t target_color[N_COLS];

    // float cur_highs[N_COLS];
    // float cur_lows[N_COLS];
    // float target_highs[N_COLS];
    // float target_lows[N_COLS];

    uint16_t *cur_color;
    uint16_t *target_color;

    float *cur_highs;
    float *cur_lows;
    float *target_highs;
    float *target_lows;
    
    uint n_cols;

    float animationspeed = 0.01;

    void interpolatecolumn(uint col, float target_high, float target_low, uint32_t deltatime)
    {
        float cur_high = this->cur_highs[col];
        float cur_low = this->cur_lows[col];

        double animspeed = this->animationspeed;
        
        double delta_high = target_high - cur_high;
        if (delta_high > 0)
        {
            this->cur_highs[col] += min(animspeed * deltatime * delta_high, delta_high);
        }
        else
        {
            this->cur_highs[col] += max(animspeed * deltatime * delta_high, delta_high);
        }

        double delta_low = target_low - cur_low;
        if (delta_low > 0)
        {
            this->cur_lows[col] += min(animspeed * deltatime * delta_low, delta_low);
        }
        else
        {
            this->cur_lows[col] += max(animspeed * deltatime * delta_low, delta_low);
        }
    }

public:
    Layer(uint n_cols_)
    {
        n_cols = n_cols_;

        cur_color = new uint16_t[n_cols];
        target_color = new uint16_t[n_cols];

        cur_highs = new float[n_cols];
        cur_lows = new float[n_cols];
        target_highs = new float[n_cols];
        target_lows = new float[n_cols];

        for (uint i = 0; i < n_cols; i++)
        {
            target_lows[i] = -1;
            target_highs[i] = -1;
            target_color[i] = 1;
        }
    }

    // Layer(uint_16_t color)
    // {
    //     for (uint i = 0; i < N_COLS; i++)
    //     {
    //         target_lows[i] = -1;
    //         target_highs[i] = -1;
    //         target_color[i] = color;
    //     }
    // }

    void clear() {
        for (uint i = 0; i < n_cols; i++)
        {
            target_lows[i] = -1;
            target_highs[i] = -1;
        }
    }

    void set_target_low(float arr[])
    {
        for (uint i = 0; i < n_cols; i++)
        {
            target_lows[i] = arr[i];
        }
    }
    void set_target_low(float f, uint idx)
    {
        target_lows[idx] = f;
    }
    void set_target_high(float arr[])
    {
        for (uint i = 0; i < n_cols; i++)
        {
            target_highs[i] = arr[i];
        }
    }
    void set_target_high(float f, uint idx)
    {
        target_highs[idx] = f;
    }
    void set_target_colors(uint16_t colors[])
    {
        for (uint i = 0; i < n_cols; i++)
        {
            target_color[i] = colors[i];
        }
    }
    void set_target_color(uint16_t color)
    {
        for (uint i = 0; i < n_cols; i++)
        {
            target_color[i] = color;
        }
    }

    void update(uint32_t deltatime, double animspeed)
    {
        this->animationspeed = animspeed;

        for (uint col = 0; col < n_cols; col++)
        {
            double cur_target_high = this->target_highs[col];
            double cur_target_low = this->target_lows[col];

            double cur_high = this->cur_highs[col];
            double cur_low = this->cur_lows[col];

            // Case 1: Nothing to do:
            if (cur_high == cur_target_high && cur_low == cur_target_low)
            {
                continue;
            }
            // Case 2: appear from nothingness:
            else if (cur_high < 0 && cur_low < 0 && cur_target_high >= 0 && cur_target_low >= 0)
            {
                double midpoint = (cur_target_low + cur_target_high) / 2;
                this->cur_lows[col] = midpoint;
                this->cur_highs[col] = midpoint;
            }
            // Case 3: disappear into nothingness:
            else if (cur_high >= 0 && cur_low >= 0 && cur_target_high < 0 && cur_target_low < 0)
            {
                double delta = abs(cur_high - cur_low);
                double midpoint = (cur_high + cur_low) / 2;
                // If close enough together, disappear:
                if (delta < DELTA_E)
                {
                    this->cur_highs[col] = -1;
                    this->cur_lows[col] = -1;
                }
                else
                { // Else, move towards midpoint
                    interpolatecolumn(col, midpoint, midpoint, deltatime);
                }
            }
            // Case 4: move normally
            else if (cur_high >= 0 && cur_low >= 0 && cur_target_high >= 0 && cur_target_low >= 0)
            {
                interpolatecolumn(col, cur_target_high, cur_target_low, deltatime);
            }
        }

        //TODO: interpolate colors
        for (uint i = 0; i < n_cols; i++)
        {
            cur_color[i] = target_color[i];
        }

    }

    inline int getLow(uint col) {
        if (cur_lows[col] < 0) {
            return -1;
        }
        return (uint)floorf(cur_lows[col] + 0.5f);
    }

    inline int getHigh(uint col) {
        if (cur_highs[col] < 0) {
            return -1;
        }
        return (uint)ceilf(cur_highs[col] + 0.5f);
    }

    inline uint16_t getColor(uint col) {
        return cur_color[col];
    }
};

#endif