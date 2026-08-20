#ifndef GLOBAL_H
#define GLOBAL_H

#include <X11/Xlib.h>

// Global window manager state
typedef struct {
    Display *dpy;
    Window root;
    int screen;
    unsigned long bg_pixel;
    unsigned long fg_pixel;
    unsigned long accent_pixel;
    unsigned long border_active;
    unsigned long border_inactive;
} WMState;

extern WMState wm;

#endif
