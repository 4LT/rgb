#include "color_ops.h"

uint32_t get_component(uint32_t rgb, color_component comp)
{
    return (rgb >> comp) & 0xFF;
}

uint32_t luminosity(uint32_t rgb)
{
    return
        get_component(rgb, RED_COMP)   * RED_SCALE   +
        get_component(rgb, GREEN_COMP) * GREEN_SCALE +
        get_component(rgb, BLUE_COMP)  * BLUE_SCALE;
}
