#include "spawn.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void spawn_command(const char *cmd) {
    if (!cmd) return;
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        setsid();
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(0);
    } else if (pid > 0) {
        // Parent - don't wait
    }
}

void spawn_terminal(void) {
    spawn_command("kitty &");
}

void spawn_rofi(void) {
    spawn_command("rofi -show run &");
}

void spawn_menu(void) {
    spawn_command("~/Documents/sweetwm/sweetmenu.py &");
}

void spawn_screenshot(void) {
    spawn_command("scrot ~/Pictures/screenshot_%Y%m%d_%H%M%S.png &");
}

void spawn_lock(void) {
    spawn_command("i3lock -c 2D1B3D &");
}

void spawn_logout(void) {
    spawn_command("pkill -KILL -u $USER");
}

void spawn_reboot(void) {
    spawn_command("systemctl reboot");
}

void spawn_shutdown(void) {
    spawn_command("systemctl poweroff");
}

void spawn_autostart(void) {
    // Try multiple autostart locations
    if (access("/home/spazmatizm/.config/sweetwm/autostart.sh", F_OK) == 0) {
        spawn_command("/home/spazmatizm/.config/sweetwm/autostart.sh &");
    } else if (access("/etc/sweetwm/autostart.sh", F_OK) == 0) {
        spawn_command("/etc/sweetwm/autostart.sh &");
    } else {
        printf("No autostart script found\n");
    }
}
