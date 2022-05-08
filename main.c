#include "canvas.h"
#include <stdbool.h>
#include <stdio.h>

static const uint32_t PIX_COUNT = 0x01000000;
static const uint32_t WIN_WIDTH = 0x00001000;

bool verify(pixel_t *pixmap)
{
    bool *vermap = calloc(PIX_COUNT, sizeof(bool));
    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        uint32_t i2 = pixmap[i] & 0x00FFFFFF;
        if (vermap[i2])
            return false;
        vermap[i2] = true;
    }
    free(vermap);
    return true;
}

int main(int argc, char *argv[])
{
    pixel_t color = 0;
    pixel_t *pixmap = malloc(sizeof(pixel_t) * PIX_COUNT);
    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        color = (color + 17*257*4097) | 0xFF000000;
        pixmap[i] = color;
    }
    if (!verify(pixmap)) {
        fprintf(stderr, "Repeated color!\n");
        return 1;
    }
    return draw(WIN_WIDTH, WIN_WIDTH, pixmap);
}
