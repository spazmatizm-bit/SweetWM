#include "composite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Simple software compositor (no GLX dependency)
typedef struct Client {
    Window window;
    int x, y;
    int width, height;
    bool mapped;
    struct Client *next;
} Client;

static Client *clients = NULL;
static Display *g_dpy = NULL;
static Window g_root = 0;
static CompositorConfig config = {
    .enabled = true,
    .gpu_accelerated = false,
    .default_opacity = 1.0,
    .blur_radius = 0.0,
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

// ============================================================
// COMPOSITOR INIT
// ============================================================

bool compositor_init(Display *dpy, Window root, CompositorConfig *cfg) {
    if (cfg) {
        config = *cfg;
    }
    
    g_dpy = dpy;
    g_root = root;
    
    printf("Compositor: initialized (software rendering)\n");
    printf("Compositor: effects - blur: %.1f, corners: %.1f, shadows: %s\n",
           config.blur_radius, config.corner_radius,
           config.shadows ? "on" : "off");
    
    return true;
}

void compositor_cleanup(void) {
    printf("Compositor: cleaning up\n");
}

// ============================================================
// CLIENT MANAGEMENT
// ============================================================

static Client* find_client(Window window) {
    Client *c = clients;
    while (c) {
        if (c->window == window) return c;
        c = c->next;
    }
    return NULL;
}

void compositor_add_window(Display *dpy, Window window) {
    if (!config.enabled) return;
    if (find_client(window)) return;
    
    Client *c = calloc(1, sizeof(Client));
    c->window = window;
    c->mapped = true;
    
    XWindowAttributes attrs;
    if (XGetWindowAttributes(dpy, window, &attrs)) {
        c->x = attrs.x;
        c->y = attrs.y;
        c->width = attrs.width;
        c->height = attrs.height;
    }
    
    c->next = clients;
    clients = c;
    
    printf("Compositor: added window 0x%lx\n", window);
}

void compositor_remove_window(Display *dpy, Window window) {
    Client **prev = &clients;
    Client *c = clients;
    
    while (c) {
        if (c->window == window) {
            *prev = c->next;
            free(c);
            printf("Compositor: removed window 0x%lx\n", window);
            return;
        }
        prev = &c->next;
        c = c->next;
    }
}

void compositor_map_window(Display *dpy, Window window) {
    Client *c = find_client(window);
    if (c) {
        c->mapped = true;
        printf("Compositor: mapped window 0x%lx\n", window);
    }
}

void compositor_unmap_window(Display *dpy, Window window) {
    Client *c = find_client(window);
    if (c) {
        c->mapped = false;
        printf("Compositor: unmapped window 0x%lx\n", window);
    }
}

void compositor_update_window(Display *dpy, Window window, int x, int y, int w, int h) {
    Client *c = find_client(window);
    if (c) {
        c->x = x;
        c->y = y;
        c->width = w;
        c->height = h;
    }
}

void compositor_handle_damage(Display *dpy, XDamageNotifyEvent *ev) {
    // Software compositor handles damage by redrawing
}

// ============================================================
// RENDERING
// ============================================================

void compositor_render_all(Display *dpy) {
    if (!config.enabled) return;
    
    // In software mode, we just let X handle rendering
    // But we could add simple effects here
    XFlush(dpy);
}

// ============================================================
// ANIMATIONS (Simplified)
// ============================================================

void compositor_fade_in(Window window, float duration) {
    printf("Compositor: fade in window 0x%lx (%.2fs)\n", window, duration);
}

void compositor_fade_out(Window window, float duration) {
    printf("Compositor: fade out window 0x%lx (%.2fs)\n", window, duration);
}

void compositor_slide_in(Window window, float start_x, float start_y, float duration) {
    printf("Compositor: slide in window 0x%lx (%.2fs)\n", window, duration);
}

void compositor_bounce(Window window, float duration) {
    printf("Compositor: bounce window 0x%lx (%.2fs)\n", window, duration);
}

void compositor_set_opacity(Window window, float opacity) {
    // Software opacity
}

void compositor_effect_blur(Window window, float radius) {
    config.blur_radius = radius;
}

void compositor_effect_glow(Window window, float intensity) {
    config.glow_focus = intensity > 0;
}

void compositor_set_vsync(bool enabled) {
    config.vsync = enabled;
}

void compositor_set_fps_limit(int fps) {
    config.max_fps = fps;
}

void compositor_load_config(const char *path) {
    printf("Compositor: loading config from %s\n", path);
}

Display* compositor_get_display(void) {
    return g_dpy;
}

// ============================================================
// MAIN LOOP
// ============================================================

void compositor_run_loop(Display *dpy) {
    if (!config.enabled) return;
    
    printf("Compositor: running main loop\n");
    
    XEvent ev;
    while (config.enabled) {
        // Process X events
        while (XPending(dpy)) {
            XNextEvent(dpy, &ev);
            
            switch (ev.type) {
                case ConfigureNotify:
                    compositor_update_window(dpy, ev.xconfigure.window,
                                           ev.xconfigure.x, ev.xconfigure.y,
                                           ev.xconfigure.width, ev.xconfigure.height);
                    break;
                    
                case MapNotify:
                    compositor_map_window(dpy, ev.xmap.window);
                    break;
                    
                case UnmapNotify:
                    compositor_unmap_window(dpy, ev.xunmap.window);
                    break;
                    
                case DestroyNotify:
                    compositor_remove_window(dpy, ev.xdestroywindow.window);
                    break;
            }
        }
        
        // Render
        compositor_render_all(dpy);
        
        // FPS limit
        usleep(1000000 / config.max_fps);
    }
}
