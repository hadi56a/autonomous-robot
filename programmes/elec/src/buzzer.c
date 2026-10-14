/**
 * @file buzzer.c
 * @brief Implémentation des fonctions de contrôle du buzzer
 */

#include <wiringPi.h>
#include "buzzer.h"

void buzzer_setup(void)
{
    wiringPiSetup();
    pinMode(BUZZER_PIN, OUTPUT);
}

void buzzer_beep(int duration_ms)
{
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration_ms);
    digitalWrite(BUZZER_PIN, LOW);
}
