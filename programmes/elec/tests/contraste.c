#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage : %s <numero_pin_wiringpi>\n", argv[0]);
        return 1;
    }

    int sensor_pin = atoi(argv[1]);

    if (wiringPiSetup() == -1) {
        printf("Erreur d'initialisation wiringPi\n");
        return 1;
    }

    pinMode(sensor_pin, INPUT);

    while (1) {
        int value = digitalRead(sensor_pin);

        if (value == LOW) {
            printf("[Pin %d] Surface claire (0)\n", sensor_pin);
        } else {
            printf("[Pin %d] Surface sombre (1)\n", sensor_pin);
        }
        
        delay(200); 
    }

    return 0;
}