#include <wiringPi.h>
#include <stdio.h>

#define SWITCH_PIN 4 
#define LED_PIN    0 

int main(void)
{
    wiringPiSetup();

    pinMode(SWITCH_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    while (1)
    {
        if (digitalRead(SWITCH_PIN) == HIGH) {
            digitalWrite(LED_PIN, HIGH); 
        } else {
            digitalWrite(LED_PIN, LOW);   
        }

        delay(20);
    }

    return 0;
}
