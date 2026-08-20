#include "bar.h"
#include "../global.h"
#include "../utils/utils.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Display *bar_dpy;
static Window bar_win;
static Window root_win;
static int bar_height = 28;
static int bar_position = 0; // 0 = top, 1 = bottom
static GC bar_gc;
static XFontStruct *font;
static int screen_width;
static int screen_height;

// Systray
static Window systray_win = 0;
static Atom systray_atom;

// Forward declarations
void draw_bar(void);
void handle_systray_event(XEvent *ev);

void bar_init(Display *dpy, Window root) {
    bar_dpy = dpy;
    root_win = root;
    
    screen_width = DisplayWidth(dpy, DefaultScreen(dpy));
    screen_height = DisplayHeight(dpy, DefaultScreen(dpy));
    
    // Create bar window
    XSetWindowAttributes attrs = {
        .override_redirect = True,
        .background_pixel = wm.bg_pixel,
        .event_mask = ExposureMask | ButtonPressMask | 
                      StructureNotifyMask | PropertyChangeMask
    };
    
    int bar_y = (bar_position == 0) ? 0 : screen_height - bar_height;
    
    bar_win = XCreateWindow(dpy, root, 0, bar_y, 
                           screen_width, bar_height,
                           0, 0, InputOutput,
                           DefaultVisual(dpy, DefaultScreen(dpy)),
                           CWOverrideRedirect | CWBackPixel | CWEventMask,
                           &attrs);
    
    // Create GC
    bar_gc = XCreateGC(dpy, bar_win, 0, NULL);
    
    // Load font
    font = XLoadQueryFont(dpy, "fixed");
    if (!font) {
        font = XLoadQueryFont(dpy, "6x10");
    }
    
    if (font) {
        XSetFont(dpy, bar_gc, font->fid);
    }
    
    // Map bar
    XMapWindow(dpy, bar_win);
    XRaiseWindow(dpy, bar_win);
    
    // Setup systray
    systray_atom = XInternAtom(dpy, "_NET_SYSTEM_TRAY_S0", False);
    
    printf("Bar initialized (height: %d, position: %s)\n", 
           bar_height, bar_position == 0 ? "top" : "bottom");
}

void bar_set_position(const char *pos) {
    if (strcmp(pos, "bottom") == 0) {
        bar_position = 1;
    } else {
        bar_position = 0;
    }
    
    if (bar_win) {
        int bar_y = (bar_position == 0) ? 0 : screen_height - bar_height;
        XMoveWindow(bar_dpy, bar_win, 0, bar_y);
    }
}

void bar_set_height(int height) {
    bar_height = height;
    if (bar_win) {
        int bar_y = (bar_position == 0) ? 0 : screen_height - bar_height;
        XResizeWindow(bar_dpy, bar_win, screen_width, bar_height);
        XMoveWindow(bar_dpy, bar_win, 0, bar_y);
    }
}

void draw_bar(void) {
    if (!bar_win) return;
    
    // Clear bar
    XSetForeground(bar_dpy, bar_gc, wm.bg_pixel);
    XFillRectangle(bar_dpy, bar_win, bar_gc, 0, 0, screen_width, bar_height);
    
    // Draw separator line
    XSetForeground(bar_dpy, bar_gc, wm.accent_pixel);
    int line_y = (bar_position == 0) ? bar_height - 1 : 0;
    XDrawLine(bar_dpy, bar_win, bar_gc, 0, line_y, screen_width, line_y);
    
    // Draw time
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%H:%M", tm);
    
    XSetForeground(bar_dpy, bar_gc, wm.fg_pixel);
    int text_x = screen_width - 60;
    int text_y = (bar_position == 0) ? bar_height - 8 : 18;
    XDrawString(bar_dpy, bar_win, bar_gc, text_x, text_y, time_str, strlen(time_str));
    
    // Draw SweetWM label
    XSetForeground(bar_dpy, bar_gc, wm.accent_pixel);
    XDrawString(bar_dpy, bar_win, bar_gc, 5, text_y, "SweetWM", 7);
    
    XFlush(bar_dpy);
}

void bar_update(void) {
    draw_bar();
}

void bar_handle_event(XEvent *ev) {
    if (ev->type == Expose && ev->xexpose.window == bar_win) {
        draw_bar();
    }
}

void bar_cleanup(void) {
    if (bar_win) {
        XDestroyWindow(bar_dpy, bar_win);
        bar_win = 0;
    }
    if (bar_gc) {
        XFreeGC(bar_dpy, bar_gc);
        bar_gc = 0;
    }
    if (font) {
        XFreeFont(bar_dpy, font);
        font = 0;
    }
}
