#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>
#include <stdbool.h>

// Compositor configuration
typedef struct {
    bool enabled;
    bool gpu_accelerated;
    float default_opacity;
    float blur_radius;
    float corner_radius;
    bool shadows;
    bool animations;
    float animation_duration;
    bool dim_inactive;
    float dim_amount;
    bool blur_background;
    bool glow_focus;
    int shadow_size;
    int max_fps;
    bool vsync;
} CompositorConfig;

// Initialization
bool compositor_init(Display *dpy, Window root, CompositorConfig *config);
void compositor_cleanup(void);

// Main loop
void compositor_run_loop(Display *dpy);
void compositor_render_all(Display *dpy);

// Window management
void compositor_add_window(Display *dpy, Window window);
void compositor_remove_window(Display *dpy, Window window);
void compositor_map_window(Display *dpy, Window window);
void compositor_unmap_window(Display *dpy, Window window);
void compositor_update_window(Display *dpy, Window window, int x, int y, int w, int h);

// Damage handling
void compositor_handle_damage(Display *dpy, XDamageNotifyEvent *ev);

// Effects
void compositor_fade_in(Window window, float duration);
void compositor_fade_out(Window window, float duration);
void compositor_slide_in(Window window, float start_x, float start_y, float duration);
void compositor_bounce(Window window, float duration);
void compositor_set_opacity(Window window, float opacity);
void compositor_effect_blur(Window window, float radius);
void compositor_effect_glow(Window window, float intensity);

// Performance
void compositor_set_vsync(bool enabled);
void compositor_set_fps_limit(int fps);

// Config
void compositor_load_config(const char *path);

#endif

// Get display
Display* compositor_get_display(void);
