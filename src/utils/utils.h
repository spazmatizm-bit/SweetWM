#ifndef UTILS_H
#define UTILS_H

#include <X11/Xlib.h>

int text_width(Display *dpy, const char *text, XFontStruct *font);
void draw_button(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
                 const char *text, unsigned long bg, unsigned long fg);
void draw_panel(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
                unsigned long color);
void draw_rect(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
               unsigned long color);
unsigned long get_color(Display *dpy, const char *name);
void init_utils(Display *dpy);

#endif
