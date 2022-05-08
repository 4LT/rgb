#include "canvas.h"
#include <stdbool.h>
#include <stdio.h>

static const uint32_t PIX_COUNT = 0x01000000;
static const uint32_t WIN_WIDTH = 4096;

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

void apply_color( uint32_t i,
        unsigned int x,
        unsigned int y,
        unsigned char *r,
        unsigned char *g,
        unsigned char *b)
{
    *r = x * 127;
    *g = y * 127;
    *b = ((x & 0xF00) >> 8) | ((y & 0xF00) >> 4);
}

int main(int argc, char *argv[])
{
    pixel_t *pixmap = malloc(sizeof(pixel_t) * PIX_COUNT);

    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        unsigned int x =  i & 0x00000FFF;
        unsigned int y = (i & 0x00FFF000) >> 12;
        unsigned char r;
        unsigned char g;
        unsigned char b;
        apply_color(i, x, y, &r, &g, &b);
        pixmap[i] = (r << 16) | (g << 8) | b | 0xFF000000;
    }

    if (!verify(pixmap)) {
        fprintf(stderr, "Repeated color!\n");
        return 1;
    }
    return draw(WIN_WIDTH, WIN_WIDTH, pixmap);
}
