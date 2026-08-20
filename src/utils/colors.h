#ifndef COLORS_H
#define COLORS_H

#include <X11/Xlib.h>

unsigned long hex_to_pixel(const char *hex);
void init_colors(Display *dpy);

#endif
