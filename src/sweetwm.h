#ifndef SWEETWM_H
#define SWEETWM_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_WINDOWS 100
#define BAR_HEIGHT 28
#define TITLEBAR_HEIGHT 22
#define BORDER_WIDTH 2
#define BTN_SIZE 16
#define MAX_APPS 100

typedef struct {
    Window win;
    Window frame;
    Window titlebar;
    Window close_btn;
    Window min_btn;
    Window max_btn;
    int x, y, w, h;
    char title[256];
    int mapped;
    int minimized;
    int maximized;
    int fullscreen;
    int saved_x, saved_y, saved_w, saved_h;
} Client;

typedef struct {
    char name[256];
    char exec[256];
} App;

typedef struct {
    Display *dpy;
    int screen;
    Window root;
    Window panel;
    Window taskbar;
    Window clock_win;
    Window start_btn;
    Window systray;
    Window menu_win;
    
    Client clients[MAX_WINDOWS];
    App apps[MAX_APPS];
    int app_count;
    int client_count;
    int focused;
    int running;
    int menu_visible;
    
    unsigned long bg;
    unsigned long fg;
    unsigned long accent;
    unsigned long border_active;
    unsigned long border_inactive;
    unsigned long title_bg;
    unsigned long title_fg;
    unsigned long bar_bg;
    unsigned long bar_fg;
    unsigned long bar_btn_bg;
    unsigned long bar_btn_active;
    unsigned long bar_btn_inactive;
    unsigned long btn_close;
    unsigned long btn_close_hover;
    unsigned long btn_min;
    unsigned long btn_min_hover;
    unsigned long btn_max;
    unsigned long btn_max_hover;
    unsigned long menu_bg;
    unsigned long menu_fg;
    unsigned long menu_hover;
    
    int bar_height;
    int bar_position;
    int border_width;
    int title_height;
    
    GC gc;
    XFontStruct *font;
    Atom wm_delete_window;
    Atom wm_protocols;
    
    int dragging;
    Window drag_win;
    int drag_x, drag_y;
    int drag_start_x, drag_start_y;
    
    int resizing;
    Window resize_win;
    int resize_x, resize_y;
    int resize_start_x, resize_start_y;
    int resize_start_w, resize_start_h;
} SweetWM;

extern SweetWM wm;

// config.c
void config_load(const char *path);
void config_free(void);
const char* config_get(const char *section, const char *key, const char *default_val);
int config_get_int(const char *section, const char *key, int default_val);
int config_get_bool(const char *section, const char *key, int default_val);

// utils.c
unsigned long hex_to_pixel(const char *hex);
int text_width(const char *str);
void draw_button(Window btn, int w, int h, unsigned long color, const char *label);

// colors.c
void init_colors(void);

// bar.c
void create_bar(void);
void draw_panel(void);

// systray.c
void systray_init(void);
void systray_handle_client_message(XClientMessageEvent *ev);
void systray_draw(void);

// window.c
void create_frame(Client *c);
void manage_window(Window win);
Client* find_client(Window win);
void focus_window(Client *c);
void unfocus_all(void);
void close_window(Client *c);
void minimize_window(Client *c);
void maximize_window(Client *c);
void remove_window(Window win);
void start_drag(Client *c, XButtonEvent *ev);
void move_drag(Client *c, XMotionEvent *ev);
void stop_drag(void);
void start_resize(Client *c, XButtonEvent *ev);
void move_resize(Client *c, XMotionEvent *ev);
void stop_resize(void);
void toggle_fullscreen(Client *c);

// spawn.c
void spawn_xterm(void);
void spawn_kitty(void);
void spawn_command(const char *cmd);
void spawn_shutdown(void);
void spawn_reboot(void);
void spawn_logout(void);
void spawn_autostart(void);

// menu.c
void load_apps(void);
void show_menu(int x, int y);
void hide_menu(void);
void draw_menu(void);
void menu_keypress(KeySym ks, char key);
void filter_apps(const char *query);
void menu_click(int x, int y);

#endif
void systray_handle_map_request(XMapRequestEvent *ev);
void systray_init(void);
void systray_init(void);
void systray_handle_client_message(XClientMessageEvent *ev);
void systray_handle_map_request(XMapRequestEvent *ev);
void systray_draw(void);
void systray_handle_map_request(XMapRequestEvent *ev);
