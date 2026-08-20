#ifndef MENU_H
#define MENU_H

#include <X11/Xlib.h>

void show_menu(Display *dpy, Window root);
void hide_menu(void);
int menu_is_active(void);
void menu_keypress(XKeyEvent *ev);
void menu_click(XButtonEvent *ev);

#endif
