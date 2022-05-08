#ifndef COLOR_OPS_H_
#define COLOR_OPS_H_

enum color_component
{
    RED_COMP = 16;
    GREEN_COMP = 8;
    BLUE_COMP = 0;
}

/* NOTE: use all primes */
const uint32_t RED_SCALE = 29;
const uint32_t GREEN_SCALE = 59;
const uint32_t BLUE_SCALE = 11;

uint32_t get_component(uint32_t rgb, color_component comp);

uint32_t luminosity(uint32_t rgb);

#endif
