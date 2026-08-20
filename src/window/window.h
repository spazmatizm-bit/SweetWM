#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>

void window_manager_init(Display *dpy, Window root);
void handle_window_event(XEvent *ev);
void focus_window(Window win);

#endif
