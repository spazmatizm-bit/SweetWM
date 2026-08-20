#include "window.h"
#include "../compositor/composite.h"
#include "../utils/utils.h"
#include "../global.h"
#include "../spawn/spawn.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FRAME_BORDER 2
#define TITLE_HEIGHT 22
#define FRAME_OFFSET_X FRAME_BORDER
#define FRAME_OFFSET_Y (TITLE_HEIGHT + FRAME_BORDER)

// Fixed struct definition - using typedef properly
typedef struct WindowInfo {
    Window window;
    Window frame;
    int x, y;
    int width, height;
    int border_width;
    int fullscreen;
    char *title;
    struct WindowInfo *next;
} WindowInfo;

static Display *dpy;
static Window root;
static WindowInfo *windows = NULL;
static Window focused_window = 0;
static int window_count = 0;

// Forward declarations
WindowInfo* find_window_info(Window win);
WindowInfo* find_window_info_by_frame(Window frame);
Window find_window_at(Window win);
void focus_window(Window win);
void update_window_frame(WindowInfo *info);
void handle_button_press(XButtonEvent *ev);
void toggle_fullscreen(Window frame);

void window_manager_init(Display *display, Window root_window) {
    dpy = display;
    root = root_window;
    
    XSelectInput(dpy, root, 
        SubstructureNotifyMask | SubstructureRedirectMask |
        PropertyChangeMask | FocusChangeMask |
        KeyPressMask | ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask);
    
    printf("Window manager initialized\n");
}

WindowInfo* find_window_info(Window win) {
    WindowInfo *info = windows;
    while (info) {
        if (info->window == win) return info;
        info = info->next;
    }
    return NULL;
}

WindowInfo* find_window_info_by_frame(Window frame) {
    WindowInfo *info = windows;
    while (info) {
        if (info->frame == frame) return info;
        info = info->next;
    }
    return NULL;
}

Window find_window_at(Window win) {
    Window parent, root_check;
    Window *children;
    unsigned int nchildren;
    
    WindowInfo *info = find_window_info_by_frame(win);
    if (info) return info->window;
    
    info = find_window_info(win);
    if (info) return win;
    
    if (XQueryTree(dpy, win, &root_check, &parent, &children, &nchildren)) {
        if (children) XFree(children);
        if (parent == root) return win;
        
        info = find_window_info_by_frame(parent);
        if (info) return info->window;
        
        Window current = win;
        Window next_parent = parent;
        while (next_parent != root && next_parent != None) {
            current = next_parent;
            info = find_window_info_by_frame(current);
            if (info) return info->window;
            XQueryTree(dpy, current, &root_check, &next_parent, &children, &nchildren);
            if (children) XFree(children);
        }
        return current;
    }
    return win;
}

char* get_window_title(Window win) {
    char *title = NULL;
    XFetchName(dpy, win, &title);
    if (!title) {
        Atom net_wm_name = XInternAtom(dpy, "_NET_WM_NAME", False);
        Atom actual_type;
        int actual_format;
        unsigned long nitems, bytes_after;
        unsigned char *prop = NULL;
        
        if (XGetWindowProperty(dpy, win, net_wm_name, 0, 1024, False,
                               XA_STRING, &actual_type, &actual_format,
                               &nitems, &bytes_after, &prop) == Success) {
            if (prop) {
                title = strdup((char*)prop);
                XFree(prop);
            }
        }
    }
    return title;
}

void update_window_frame(WindowInfo *info) {
    if (!info || info->fullscreen) return;
    
    int frame_x = info->x - FRAME_OFFSET_X;
    int frame_y = info->y - FRAME_OFFSET_Y;
    int frame_width = info->width + FRAME_BORDER * 2;
    int frame_height = info->height + TITLE_HEIGHT + FRAME_BORDER * 2;
    
    XResizeWindow(dpy, info->frame, frame_width, frame_height);
    XMoveWindow(dpy, info->frame, frame_x, frame_y);
    XMoveResizeWindow(dpy, info->window, 
                      FRAME_OFFSET_X, FRAME_OFFSET_Y,
                      info->width, info->height);
    
    GC gc = XCreateGC(dpy, info->frame, 0, NULL);
    XSetForeground(dpy, gc, wm.bg_pixel);
    XFillRectangle(dpy, info->frame, gc, 0, 0, frame_width, TITLE_HEIGHT);
    XSetForeground(dpy, gc, wm.accent_pixel);
    XDrawLine(dpy, info->frame, gc, 0, TITLE_HEIGHT - 1, frame_width, TITLE_HEIGHT - 1);
    
    if (info->title) {
        XSetForeground(dpy, gc, wm.fg_pixel);
        XDrawString(dpy, info->frame, gc, 5, TITLE_HEIGHT - 6, 
                    info->title, strlen(info->title));
    }
    XFreeGC(dpy, gc);
}

void manage_window(Window win) {
    XWindowAttributes attrs;
    XGetWindowAttributes(dpy, win, &attrs);
    
    if (find_window_info(win)) return;
    
    char *title = get_window_title(win);
    WindowInfo *info = calloc(1, sizeof(WindowInfo));
    info->window = win;
    info->x = attrs.x;
    info->y = attrs.y;
    info->width = attrs.width;
    info->height = attrs.height;
    info->title = title;
    info->fullscreen = 0;
    
    int frame_width = attrs.width + FRAME_BORDER * 2;
    int frame_height = attrs.height + TITLE_HEIGHT + FRAME_BORDER * 2;
    int frame_x = attrs.x - FRAME_OFFSET_X;
    int frame_y = attrs.y - FRAME_OFFSET_Y;
    if (frame_x < 0) frame_x = 0;
    if (frame_y < 0) frame_y = 0;
    
    Window frame = XCreateSimpleWindow(dpy, root, 
        frame_x, frame_y, frame_width, frame_height,
        2, wm.border_active, wm.bg_pixel);
    
    XStoreName(dpy, frame, title ? title : "Window");
    
    XSelectInput(dpy, frame, 
        EnterWindowMask | FocusChangeMask | 
        PropertyChangeMask | KeyPressMask |
        ButtonPressMask | ButtonReleaseMask | 
        StructureNotifyMask | ExposureMask |
        PointerMotionMask | SubstructureRedirectMask);
    
    XReparentWindow(dpy, win, frame, FRAME_OFFSET_X, FRAME_OFFSET_Y);
    
    XSelectInput(dpy, win,
        EnterWindowMask | FocusChangeMask |
        ButtonPressMask | ButtonReleaseMask |
        PropertyChangeMask | StructureNotifyMask);
    
    info->frame = frame;
    info->next = windows;
    windows = info;
    window_count++;
    
    XMapWindow(dpy, frame);
    compositor_add_window(dpy, frame);
    focus_window(frame);
    update_window_frame(info);
    
    printf("Window managed: 0x%lx (frame: 0x%lx) '%s'\n", win, frame, title ? title : "(no title)");
}

void remove_window(Window win) {
    WindowInfo *prev = NULL;
    WindowInfo *info = windows;
    
    while (info) {
        if (info->window == win || info->frame == win) {
            if (prev) {
                prev->next = info->next;
            } else {
                windows = info->next;
            }
            compositor_remove_window(dpy, info->frame);
            if (focused_window == info->frame) focused_window = 0;
            free(info->title);
            free(info);
            window_count--;
            printf("Window removed\n");
            return;
        }
        prev = info;
        info = info->next;
    }
}

void focus_window(Window win) {
    if (!win) {
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
        focused_window = 0;
        return;
    }
    
    XWindowAttributes attrs;
    if (!XGetWindowAttributes(dpy, win, &attrs)) return;
    
    if (focused_window == win) return;
    
    if (focused_window) {
        WindowInfo *old_info = find_window_info_by_frame(focused_window);
        if (old_info) {
            XSetWindowBorder(dpy, focused_window, wm.border_inactive);
        }
    }
    
    XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
    XRaiseWindow(dpy, win);
    
    WindowInfo *info = find_window_info_by_frame(win);
    if (info) {
        XSetWindowBorder(dpy, win, wm.border_active);
    }
    
    focused_window = win;
    printf("Focused: 0x%lx\n", win);
}

void toggle_fullscreen(Window frame) {
    WindowInfo *info = find_window_info_by_frame(frame);
    if (!info) return;
    
    info->fullscreen = !info->fullscreen;
    
    if (info->fullscreen) {
        XMoveResizeWindow(dpy, info->frame, 0, 0, 
                         DisplayWidth(dpy, DefaultScreen(dpy)),
                         DisplayHeight(dpy, DefaultScreen(dpy)));
        XMoveResizeWindow(dpy, info->window, 0, 0,
                         DisplayWidth(dpy, DefaultScreen(dpy)),
                         DisplayHeight(dpy, DefaultScreen(dpy)));
        XSetWindowBorderWidth(dpy, info->frame, 0);
        XRaiseWindow(dpy, info->frame);
        printf("Fullscreen ON\n");
    } else {
        XSetWindowBorderWidth(dpy, info->frame, 2);
        update_window_frame(info);
        printf("Fullscreen OFF\n");
    }
}

void handle_button_press(XButtonEvent *ev) {
    Window target = ev->window;
    
    if (target == root) {
        focus_window(0);
        return;
    }
    
    Window top = find_window_at(target);
    if (top != root && top != None) {
        WindowInfo *info = find_window_info(top);
        if (info) {
            focus_window(info->frame);
        }
    }
}

void handle_configure_request(XConfigureRequestEvent *cre) {
    WindowInfo *info = find_window_info(cre->window);
    if (info) {
        info->x = cre->x;
        info->y = cre->y;
        info->width = cre->width;
        info->height = cre->height;
        update_window_frame(info);
    } else {
        XWindowChanges changes = {
            .x = cre->x,
            .y = cre->y,
            .width = cre->width,
            .height = cre->height,
            .border_width = cre->border_width,
            .sibling = cre->above,
            .stack_mode = cre->detail
        };
        XConfigureWindow(dpy, cre->window, cre->value_mask, &changes);
    }
}

void handle_key_press(XKeyEvent *ev) {
    KeySym keysym = XLookupKeysym(ev, 0);
    
    if (keysym == XK_F11) {
        Window win = find_window_at(ev->window);
        WindowInfo *info = find_window_info(win);
        if (info) {
            toggle_fullscreen(info->frame);
        }
        return;
    }
    
    if ((ev->state & Mod4Mask) && keysym == XK_Q) {
        Window win = find_window_at(ev->window);
        WindowInfo *info = find_window_info(win);
        if (info) {
            XDestroyWindow(dpy, info->frame);
        }
        return;
    }
    
    if ((ev->state & ControlMask) && (ev->state & ShiftMask) && keysym == XK_T) {
        spawn_terminal();
        return;
    }
    
    if ((ev->state & Mod4Mask) && keysym == XK_space) {
        spawn_menu();
        return;
    }
}

void handle_window_event(XEvent *ev) {
    switch (ev->type) {
        case CreateNotify:
            break;
            
        case MapRequest:
            if (ev->xmaprequest.window != root) {
                if (!find_window_info(ev->xmaprequest.window)) {
                    manage_window(ev->xmaprequest.window);
                } else {
                    XMapWindow(dpy, ev->xmaprequest.window);
                }
            }
            break;
            
        case MapNotify:
            break;
            
        case UnmapNotify:
            remove_window(ev->xunmap.window);
            break;
            
        case DestroyNotify:
            remove_window(ev->xdestroywindow.window);
            break;
            
        case ConfigureRequest:
            handle_configure_request(&ev->xconfigurerequest);
            break;
            
        case ConfigureNotify:
            break;
            
        case ButtonPress:
            handle_button_press(&ev->xbutton);
            break;
            
        case ButtonRelease:
            break;
            
        case KeyPress:
            handle_key_press(&ev->xkey);
            break;
            
        case KeyRelease:
            break;
            
        case FocusIn:
            break;
            
        case FocusOut:
            break;
            
        case EnterNotify:
            break;
            
        case Expose:
            if (ev->xexpose.count == 0) {
                WindowInfo *info = find_window_info_by_frame(ev->xexpose.window);
                if (info) {
                    update_window_frame(info);
                }
            }
            break;
    }
}
