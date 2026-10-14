#include <wiringPi.h>
#include <stdio.h>

#define BUZZER_PIN 1  

int main(void)
{
    if (wiringPiSetup() == -1) {
        printf("Erreur d'initialisation wiringPi\n");
        return 1;
    }
 
    pinMode(BUZZER_PIN, OUTPUT);

    while (1) {
        digitalWrite(BUZZER_PIN, HIGH);  
        delay(500);                     

        digitalWrite(BUZZER_PIN, LOW);  
        delay(500);                      
    }

    return 0;
}