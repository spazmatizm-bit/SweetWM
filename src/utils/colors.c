#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple color conversion
unsigned long hex_to_pixel(const char *hex) {
    if (!hex || hex[0] != '#') return 0;
    
    unsigned long r, g, b;
    if (strlen(hex) == 7) {
        sscanf(hex + 1, "%02lx%02lx%02lx", &r, &g, &b);
    } else if (strlen(hex) == 4) {
        sscanf(hex + 1, "%01lx%01lx%01lx", &r, &g, &b);
        r = (r << 4) | r;
        g = (g << 4) | g;
        b = (b << 4) | b;
    } else {
        return 0;
    }
    
    return (r << 16) | (g << 8) | b;
}

void init_colors(Display *dpy) {
    // Colors are handled by the theme system
    printf("Colors initialized\n");
}
