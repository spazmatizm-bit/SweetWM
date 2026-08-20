#include "menu.h"
#include "../global.h"
#include "../spawn/spawn.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Display *menu_dpy = NULL;
static Window menu_win = 0;
static int menu_active = 0;

void show_menu(Display *dpy, Window root) {
    menu_dpy = dpy;
    menu_active = 1;
    
    // Simple menu implementation
    // For now, just spawn the python menu
    spawn_menu();
}

void hide_menu(void) {
    menu_active = 0;
    if (menu_win) {
        XDestroyWindow(menu_dpy, menu_win);
        menu_win = 0;
    }
}

int menu_is_active(void) {
    return menu_active;
}

void menu_keypress(XKeyEvent *ev) {
    // Handle menu keypresses
}

void menu_click(XButtonEvent *ev) {
    // Handle menu clicks
}
