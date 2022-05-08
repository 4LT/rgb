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

uint32_t shift(uint32_t in)
{
    pixel_t r = (in & 0xFF0000) >> 16;
    pixel_t g = (in & 0xFF00) >> 8;
    pixel_t b = (in & 0xFF);
    pixel_t r2 = (r ^ 0x36) & 0xFF;
    pixel_t g2 = (g ^ 0x36) & 0xFF;
    pixel_t b2 = ((b^0x36  + r*135 )) & 0xFF;
    return  (r2<<16) | (g2<<8) | b2;
}

int main(int argc, char *argv[])
{
    pixel_t *pixmap = malloc(sizeof(pixel_t) * PIX_COUNT);

    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        pixmap[i] = shift((i ^ 0x13579BDF)*4097);
        pixmap[i] = pixmap[i] | 0xFF000000;
    }

    if (!verify(pixmap)) {
        fprintf(stderr, "Repeated color!\n");
        return 1;
    }
    return draw(WIN_WIDTH, WIN_WIDTH, pixmap);
}
