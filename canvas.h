#ifndef CANVAS_H_
#define CANVAS_H_

#include <stdint.h>
#include <stdlib.h>

typedef uint32_t  pixel_t;

/* Draws image to screen.
 * pixmap - final image in RGBA8888 pixels
 */
int draw(size_t width, size_t height, pixel_t *pixmap);

#endif
