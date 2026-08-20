#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "compositor/composite.h"
#include "window/window.h"
#include "spawn/spawn.h"
#include "utils/colors.h"
#include "utils/utils.h"
#include "menu/menu.h"
#include "global.h"

static Display *dpy;
static Window root;
static CompositorConfig comp_config = {
    .enabled = true,
    .gpu_accelerated = false,
    .default_opacity = 1.0,
    .blur_radius = 5.0,
    .corner_radius = 8.0,
    .shadows = true,
    .animations = true,
    .animation_duration = 0.2,
    .dim_inactive = true,
    .dim_amount = 0.3,
    .blur_background = false,
    .glow_focus = true,
    .shadow_size = 10,
    .max_fps = 60,
    .vsync = true
};

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nSweetWM: shutting down...\n");
        compositor_cleanup();
        XCloseDisplay(dpy);
        exit(0);
    }
    if (sig == SIGHUP) {
        printf("SweetWM: reloading config...\n");
        compositor_load_config("~/.config/sweetwm/config.swwm");
    }
}

int main(int argc, char **argv) {
    // Open display
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    
    root = DefaultRootWindow(dpy);
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    
    printf("SweetWM: starting...\n");
    printf("Display: %s\n", DisplayString(dpy));
    
    // Initialize utilities
    init_utils(dpy);
    
    // Initialize colors
    init_colors(dpy);
    
    // Initialize compositor
    if (!compositor_init(dpy, root, &comp_config)) {
        fprintf(stderr, "Compositor: falling back to non-composited mode\n");
        comp_config.enabled = false;
    }
    
    // Initialize window manager
    window_manager_init(dpy, root);
    
    // Load startup apps
    spawn_autostart();
    
    printf("SweetWM: ready!\n");
    printf("================================\n");
    printf("Hotkeys:\n");
    printf("  Ctrl+Shift+T - terminal\n");
    printf("  Mod4+Q - close window\n");
    printf("  Mod4+Tab - switch window\n");
    printf("  Mod4+M - minimize\n");
    printf("  Mod4+F - maximize\n");
    printf("  Mod4+F11 - fullscreen\n");
    printf("  Mod4+Space - menu\n");
    printf("================================\n");
    
    // Main event loop
    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        
        // Handle window events
        handle_window_event(&ev);
        
        // Handle menu events
        if (ev.type == KeyPress) {
            menu_keypress(&ev.xkey);
        }
        if (ev.type == ButtonPress) {
            menu_click(&ev.xbutton);
        }
    }
    
    // Cleanup
    compositor_cleanup();
    XCloseDisplay(dpy);
    
    return 0;
}
