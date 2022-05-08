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
static const int RED_SCALE = 30;
static const int GREEN_SCALE = 59;
static const int BLUE_SCALE = 11;
static const pixel_t BRIGHTEST = PIX_COUNT - 1;
static const pixel_t DARKEST = 0;

enum split_dir
{
    KD_UP,
    KD_LEFT,
    KD_DOWN,
    KD_RIGHT
};

struct region
{
    int x;
    int y;
    int w;
    int h;
    int min;
    int max;
};

struct point
{
    int x;
    int y;
};

enum split_dir next_dir(enum split_dir dir)
{ return (dir+1) % 4; }
enum split_dir prev_dir(enum split_dir dir)
{ return (dir+3) % 4; }
bool split_horiz(enum split_dir dir)
{ return dir == KD_LEFT || dir == KD_RIGHT; }
bool split_positive(enum split_dir dir)
{ return dir == KD_RIGHT || dir == KD_UP; }

void shuffle(pixel_t *pixmap, int length)
{
    for (int shuf_len = length; shuf_len > 0; shuf_len--) {
        int select = rand() % shuf_len;
        MS_SWAP(pixel_t, pixmap[select], pixmap[shuf_len - 1]);
    }
}

int brightness(pixel_t pix)
{
    return ((pix >> 16) & 0xFF) * RED_SCALE +
           ((pix >>  8) & 0xFF) * GREEN_SCALE +
           ((pix      ) & 0xFF) * BLUE_SCALE;
}

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

bool pixel_lt(pixel_t pix1, pixel_t pix2)
{
    bool lt;
    if (brightness(pix1) < brightness(pix2))
        lt = true;
    else if (brightness(pix1) > brightness(pix2))
        lt = false;
    else
        lt = interleave_rgb(pix1) < interleave_rgb(pix2);
    return lt;
}

struct region root_region()
{
    return (struct region) { 0, 0, WIN_WIDTH, WIN_WIDTH, DARKEST, BRIGHTEST };
}

void print_region(struct region r)
{
    printf("pos:     %8d %8d\n", r.x,   r.y);
    printf("dim:     %8d %8d\n", r.w,   r.h);
    printf("bounds:  %8d %8d\n", r.min, r.max);
}

int abs_pos(struct region r, int x, int y)
{ return (r.y + y) * WIN_WIDTH + (r.x + x); }
struct point rel_pos(struct region r, int abs)
{ return (struct point) { abs % WIN_WIDTH - r.x, abs / WIN_WIDTH - r.y }; }

void my_qsort(pixel_t *pixlist, int start, int end)
{
    int len = end - start + 1;

    if (len < 2)
        return;

    int l = start;
    int r = end - 1;
    int pivotIdx = start + len/2;
    int pivot = pixlist[pivotIdx];
    MS_SWAP(pixel_t, pixlist[pivotIdx], pixlist[end])

    while (l < r) {
        if (pixel_lt(pixlist[l], pivot)) {
            l++;
        }
        else {
            MS_SWAP(pixel_t, pixlist[l], pixlist[r])
            r--;
        }
    }

    if (pixel_lt(pixlist[l], pivot)) {
        my_qsort(pixlist, start, l);
    }
    else {
        MS_SWAP(pixel_t, pixlist[l], pixlist[end])
        my_qsort(pixlist, start, l-1);
    }

    my_qsort(pixlist, l+1, end);
}

void split(pixel_t *pixmap, pixel_t const *pixlist, enum split_dir dir,
        struct region in)
{
    if (in.w == 1 && in.h == 1) {
        pixmap[abs_pos(in, 0, 0)] = pixlist[in.min];
        return;
    }
    else if (in.w == 1 && split_horiz(dir))
        dir = next_dir(dir);
    else if (in.h == 1 && !split_horiz(dir))
        dir = next_dir(dir);

    struct region low, high;
    int split_low, split_high;
    if (split_horiz(dir)) {
        split_low = rand() % (in.w - 1) + 1;
        split_high = in.w - split_low;
        low = (struct region) { in.x, in.y,
            split_low, in.h,
            in.min, in.min + split_low * in.h - 1 };
        high = (struct region) { split_low + in.x, in.y,
            split_high, in.h,
            in.max - split_high * in.h + 1, in.max };
    }
    else {
        split_low = rand() % (in.h - 1) + 1;
        split_high = in.h - split_low;
        low = (struct region) { in.x, in.y,
            in.w, split_low,
            in.min, in.min + split_low * in.w - 1 };
        high = (struct region) { in.x, split_low + in.y,
            in.w, split_high,
            in.max - split_high * in.w + 1, in.max };
    }

    if (!split_positive(dir)) {
        MS_SWAP(struct region, low, high)
        low.max = in.min + in.max - low.min;
        high.min = low.max + 1;
        low.min = in.min;
        high.max = in.max;
    }
        
    split(pixmap, pixlist, prev_dir(dir), low);
    split(pixmap, pixlist, next_dir(dir), high);
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
    srand(19);
    pixel_t *pixmap = malloc(sizeof(pixel_t) * PIX_COUNT);
    pixel_t *pixlist = malloc(sizeof(pixel_t) * PIX_COUNT);
    for (uint32_t i = 0; i < PIX_COUNT; i++) {
        pixlist[i] = i;
    }
    my_qsort(pixlist, 0, PIX_COUNT - 1);
    //shuffle(pixmap, PIX_COUNT);
    split(pixmap, pixlist, KD_RIGHT, root_region());
    if (!verify(pixmap)) {
        fprintf(stderr, "Repeated color!\n");
        return 1;
    }
    return draw(WIN_WIDTH, WIN_WIDTH, pixmap);
    free(pixlist);
    free(pixmap);
}
