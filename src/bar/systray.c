#include "../sweetwm.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define SYSTEM_TRAY_REQUEST_DOCK 0
#define XEMBED_EMBEDDED_NOTIFY 0

static Atom xembed_atom;
static Atom net_system_tray_opcode;

void systray_init() {
    xembed_atom = XInternAtom(wm.dpy, "_XEMBED", False);
    net_system_tray_opcode = XInternAtom(wm.dpy, "_NET_SYSTEM_TRAY_OPCODE", False);
    
    Atom net_system_tray_selection = XInternAtom(wm.dpy, "_NET_SYSTEM_TRAY_S0", False);
    XSetSelectionOwner(wm.dpy, net_system_tray_selection, wm.systray, CurrentTime);
    XFlush(wm.dpy);
    
    Atom manager_atom = XInternAtom(wm.dpy, "MANAGER", False);
    XClientMessageEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = ClientMessage;
    ev.window = wm.root;
    ev.message_type = manager_atom;
    ev.format = 32;
    ev.data.l[0] = CurrentTime;
    ev.data.l[1] = net_system_tray_selection;
    ev.data.l[2] = wm.systray;
    XSendEvent(wm.dpy, wm.root, False, SubstructureNotifyMask, (XEvent*)&ev);
    XFlush(wm.dpy);
    
    // Устанавливаем нормальный фон для трея
    XSetWindowBackground(wm.dpy, wm.systray, wm.bar_bg);
    XClearWindow(wm.dpy, wm.systray);
    
    printf("Systray: initialized!\n");
}

void systray_handle_client_message(XClientMessageEvent *ev) {
    if (ev->message_type == net_system_tray_opcode) {
        long opcode1 = ev->data.l[0];
        long opcode2 = ev->data.l[1];
        Window client = ev->data.l[2];
        
        if (opcode1 == SYSTEM_TRAY_REQUEST_DOCK || opcode2 == SYSTEM_TRAY_REQUEST_DOCK) {
            XReparentWindow(wm.dpy, client, wm.systray, 2, 2);
            XMapWindow(wm.dpy, client);
            XRaiseWindow(wm.dpy, client);
            XFlush(wm.dpy);
            printf("Systray: docked!\n");
        }
        return;
    }
}

void systray_handle_map_request(XMapRequestEvent *ev) {
    if (!wm.systray) return;
    
    Atom net_wm_type = XInternAtom(wm.dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = NULL;
    
    if (XGetWindowProperty(wm.dpy, ev->window, net_wm_type, 0, 1, False,
                          XA_ATOM, &actual_type, &actual_format,
                          &nitems, &bytes_after, &prop) == Success && prop) {
        Atom *types = (Atom*)prop;
        Atom tray_type = XInternAtom(wm.dpy, "_NET_WM_WINDOW_TYPE_SYSTEM_TRAY", False);
        if (nitems > 0 && types[0] == tray_type) {
            XFree(prop);
            XReparentWindow(wm.dpy, ev->window, wm.systray, 2, 2);
            XMapWindow(wm.dpy, ev->window);
            XRaiseWindow(wm.dpy, ev->window);
            XFlush(wm.dpy);
            return;
        }
        XFree(prop);
    }
}

void systray_draw() {
    if (!wm.systray) return;
    // Просто очищаем фон
    XSetForeground(wm.dpy, wm.gc, wm.bar_bg);
    XFillRectangle(wm.dpy, wm.systray, wm.gc, 0, 0, 100, wm.bar_height-4);
}
