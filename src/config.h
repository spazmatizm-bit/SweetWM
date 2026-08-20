#ifndef CONFIG_H
#define CONFIG_H

#include <X11/Xlib.h>
#include <stdbool.h>

// Config functions
void config_load(const char *path);
void config_reload(void);
char* config_get_string(const char *key, const char *default_value);
int config_get_int(const char *key, int default_value);
bool config_get_bool(const char *key, bool default_value);
float config_get_float(const char *key, float default_value);

// Window manager functions (to be implemented in window.c)
void window_manager_init(Display *dpy, Window root);
void spawn_autostart(void);

#endif
