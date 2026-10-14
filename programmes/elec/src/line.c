/**
 * @file line.c
 * @brief Implémentation des fonctions de lecture des capteurs de ligne
 */

#include "line.h"
#include <wiringPi.h>
#include <stdbool.h>

int line_init()
{

    if (wiringPiSetup() == -1) {
        return -1;
    }

    pinMode(PIN_BACK_LEFT, INPUT);
    pinMode(PIN_FRONT_LEFT, INPUT);
    pinMode(PIN_FRONT_RIGHT, INPUT);
    pinMode(PIN_BACK_RIGHT, INPUT);
    pinMode(PIN_FRONT_CENTER, INPUT);

    return 0;
}


bool line_front_left()
{
    return digitalRead(PIN_FRONT_LEFT) == HIGH;
}

bool line_front_right()
{
    return digitalRead(PIN_FRONT_RIGHT) == HIGH;
}

bool line_front_center()
{
    return digitalRead(PIN_FRONT_CENTER) == HIGH;
}

bool line_back_left()
{
    return digitalRead(PIN_BACK_LEFT) == HIGH;
}

bool line_back_right()
{
    return digitalRead(PIN_BACK_RIGHT) == HIGH;
}
