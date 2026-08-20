#include "utils.h"
#include "../global.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global WM state
WMState wm = {0};

int text_width(Display *dpy, const char *text, XFontStruct *font) {
    if (!text || !font) return 0;
    return XTextWidth(font, text, strlen(text));
}

void draw_button(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
                 const char *text, unsigned long bg, unsigned long fg) {
    XSetForeground(dpy, gc, bg);
    XFillRectangle(dpy, win, gc, x, y, w, h);
    XSetForeground(dpy, gc, fg);
    XDrawRectangle(dpy, win, gc, x, y, w, h);
    if (text) {
        XDrawString(dpy, win, gc, x + 5, y + h - 5, text, strlen(text));
    }
}

void draw_panel(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
                unsigned long color) {
    XSetForeground(dpy, gc, color);
    XFillRectangle(dpy, win, gc, x, y, w, h);
}

void draw_rect(Display *dpy, Window win, GC gc, int x, int y, int w, int h,
               unsigned long color) {
    XSetForeground(dpy, gc, color);
    XFillRectangle(dpy, win, gc, x, y, w, h);
}

unsigned long get_color(Display *dpy, const char *name) {
    XColor color, dummy;
    if (XAllocNamedColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)),
                         name, &color, &dummy)) {
        return color.pixel;
    }
    return 0;
}

void init_utils(Display *dpy) {
    wm.dpy = dpy;
    wm.root = DefaultRootWindow(dpy);
    wm.screen = DefaultScreen(dpy);
    wm.bg_pixel = get_color(dpy, "#2D1B3D");
    wm.fg_pixel = get_color(dpy, "#E1BEE7");
    wm.accent_pixel = get_color(dpy, "#9B4DCA");
    wm.border_active = get_color(dpy, "#9B4DCA");
    wm.border_inactive = get_color(dpy, "#4A2B5D");
    printf("Utils initialized\n");
}
