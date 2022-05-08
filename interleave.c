#include "canvas.h"
#include <stdbool.h>
#include <stdio.h>

#define MS_SWAP(type, a, b) {\
    type temp__ = (a);\
    (a) = (b);\
    (b) = temp__;\
}

static const uint32_t PIX_COUNT = 0x01000000;
static const uint32_t WIN_WIDTH = 0x00001000;


pixel_t interleave_rgb(pixel_t in)
{
    pixel_t out = 0;
    for (int i = 0; i < 8; i++) {
        int out_offset = 3 * i;
        int b_offset = i;
        int g_offset = i + 8;
        int r_offset = i + 16;
        int bbit = ((1 << b_offset) & in) >> b_offset;
        int gbit = ((1 << g_offset) & in) >> g_offset;
        int rbit = ((1 << r_offset) & in) >> r_offset;
        out = out | (bbit <<  out_offset     );
        out = out | (gbit << (out_offset + 1));
        out = out | (rbit << (out_offset + 2));
    }
    return out;
}

bool verify(const pixel_t *pixmap)
{
    bool *vermap = calloc(PIX_COUNT, sizeof(bool));
    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        if (vermap[pixmap[i]]) {
            free(vermap);
            return false;
        }
        vermap[pixmap[i]] = true;
    }
    free(vermap);
    return true;
}

int main(int argc, char *argv[])
{
    pixel_t *pixmap = malloc(sizeof(pixel_t) * PIX_COUNT);
    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        pixmap[i] = interleave_rgb(i);
    }
    if (!verify(pixmap)) {
        fprintf(stderr, "Repeated color!\n");
        return 1;
    }
    return draw(WIN_WIDTH, WIN_WIDTH, pixmap);
}
