#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "mon_lcd.h"
#include "line.h"
#include "capteur_color.h"

volatile int running = 1;

void handle_sigint(int sig) {
    running = 0;
}

int main() {
    signal(SIGINT, handle_sigint);

    if (lcd_initialisation() < 0) {
        printf("Erreur init LCD\n");
        return 1;
    }
    if (line_init() < 0) {
        printf("Erreur init Line\n");
        return 1;
    }
    int color_fd = color_initialisation();
    if (color_fd < 0) {
        printf("Erreur init Color\n");
        return 1;
    }

    lcd_effacer();
    
    while(running) {
        // Read Line Sensors
        int bl = line_back_left();
        int fl = line_front_left();
        int fc = line_front_center();
        int fr = line_front_right();
        int br = line_back_right();

        // Read Color Sensor
        int r, g, b, c;
        lire_valeurs_brutes(color_fd, &r, &g, &b, &c);

        // Format Output
        char line_str[17];
        char color_str[17];

        // L: BL FL FC FR BR
        snprintf(line_str, 17, "L:%d%d%d%d%d", bl, fl, fc, fr, br);
        
        // RGB
        snprintf(color_str, 17, "%d %d %d", r, g, b);

        // Display
        lcd_effacer();
        lcd_afficher_xy(0, 0, line_str);
        lcd_afficher_xy(0, 1, color_str);
        
        usleep(200000); // 0.2s update rate
    }
    
    lcd_effacer();
    return 0;
}
