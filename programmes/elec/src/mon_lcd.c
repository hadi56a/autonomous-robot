/**
 * @file mon_lcd.c
 * @brief Implémentation des fonctions de contrôle de l'écran LCD
 */

#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include "mon_lcd.h"

#define AF_BASE 64
#define AF_RS   (AF_BASE + 0)
#define AF_RW   (AF_BASE + 1)
#define AF_E    (AF_BASE + 2)
#define AF_LED  (AF_BASE + 3)
#define AF_DB4  (AF_BASE + 4)
#define AF_DB5  (AF_BASE + 5)
#define AF_DB6  (AF_BASE + 6)
#define AF_DB7  (AF_BASE + 7)

// Variable globale interne pour garder la poignée (handle) de l'écran
static int lcdHandle;

int lcd_initialisation() {
    // Initialisation de wiringPi si ce n'est pas déjà fait ailleurs
    if (wiringPiSetup() == -1) {
        return -1;
    }

    // Configuration de l'extension PCF8574 à l'adresse 0x27
    pcf8574Setup(AF_BASE, 0x27);

    // Initialisation de l'écran LCD via wiringPi
    lcdHandle = lcdInit(2, 16, 4, AF_RS, AF_E, AF_DB4, AF_DB5, AF_DB6, AF_DB7, 0, 0, 0, 0);

    if (lcdHandle < 0) {
        fprintf(stderr, "Erreur initialisation LCD\n");
        return -1;
    }

    // Allumer le rétroéclairage (LED) et mettre en mode écriture (RW=0)
    for (int i = 0; i < 8; i++) pinMode(AF_BASE + i, OUTPUT);
    digitalWrite(AF_LED, 1); // Lumière ON
    digitalWrite(AF_RW, 0);  // Mode écriture

    return 0;
}

void lcd_effacer() {
    lcdClear(lcdHandle);
}

void lcd_afficher(const char *message) {
    // On efface l'écran avant d'écrire pour éviter les restes de texte
    lcdClear(lcdHandle); 
    // On se place en haut à gauche
    lcdPosition(lcdHandle, 0, 0);
    // On écrit le message
    lcdPrintf(lcdHandle, message);
}

void lcd_afficher_xy(int x, int y, const char *message) {
    lcdPosition(lcdHandle, x, y);
    lcdPrintf(lcdHandle, message);
}