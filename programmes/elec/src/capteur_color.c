/**
 * @file capteur_color.c
 * @brief Implémentation de la gestion du capteur de couleur TCS34725 via I2C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_BUS "/dev/i2c-1"
#define SENSOR_ADDR 0x29  // Adresse standard du TCS34725

// Registres du capteur
#define TCS_COMMAND_BIT 0x80
#define TCS_ENABLE      0x00
#define TCS_CDATAL      0x14 // Adresse de départ des données (Clear Data Low)


// Initialisation du capteur de couleur
int color_initialisation() {
    int file;
    char *bus = I2C_BUS;

    // 1. Ouvrir le bus I2C
    if ((file = open(bus, O_RDWR)) < 0) {
        printf("Erreur : Impossible d'ouvrir le bus I2C\n");
        return 1;
    }

    // 2. Se connecter au capteur
    if (ioctl(file, I2C_SLAVE, SENSOR_ADDR) < 0) {
        printf("Erreur : Impossible de trouver le capteur à l'adresse 0x%02X\n", SENSOR_ADDR);
        return 1;
    }

    // 3. Initialiser le capteur (Power ON + ADC Enable)
    // On écrit dans le registre 0x00 la valeur 0x03 (PON + AEN)
    uint8_t config[2];
    config[0] = TCS_COMMAND_BIT | TCS_ENABLE; // Cette ligne sert à indiquer le registre
    config[1] = 0x03;  // L'adresse du capteur
    write(file, config, 2);

    // Petite pause pour laisser le capteur démarrer
    usleep(20000);

    return file;
}

//renvoie 'vert' 1 ou 'rouge' 2 ou 'indetermine' 0 en fonction de la couleur lue
int lire_couleur(int file) {
    // 4. Préparer la lecture
    // On demande à lire à partir du registre 0x14
    // 0x20 active l'auto-incrément pour lire les 8 octets à la suite
    uint8_t reg = TCS_COMMAND_BIT | 0x20 | TCS_CDATAL;
    write(file, &reg, 1);

    // 5. Lire 8 octets (Clear L/H, Red L/H, Green L/H, Blue L/H)
    uint8_t buffer[8];
    if (read(file, buffer, 8) != 8) {
        printf("Capteur couleur : erreur lors de la lecture des données\n");
        return -1;
    } else {
        // Reconstitution des valeurs sur 16 bits
        // int lumi = (buffer[1] << 8) | buffer[0];
        int r = (buffer[3] << 8) | buffer[2];
        int g = (buffer[5] << 8) | buffer[4];
        int b = (buffer[7] << 8) | buffer[6];

        //printf("r: %d, g: %d\n", r, g);
        if (r > 1.4 * g && r > 1.4 * b) {
            return 2; // rouge
        } else if (g > 1.1 * r && g > 1.1 * b) {
            return 1; // vert
        } else {
            return 0; // indetermine
        }
    }
}

void lire_valeurs_brutes(int file, int *r, int *g, int *b, int *c) {
    uint8_t reg = TCS_COMMAND_BIT | 0x20 | TCS_CDATAL;
    write(file, &reg, 1);

    uint8_t buffer[8];
    if (read(file, buffer, 8) != 8) {
        printf("Capteur couleur : erreur lors de la lecture des données\n");
        *r = *g = *b = *c = -1;
    } else {
        *c = (buffer[1] << 8) | buffer[0];
        *r = (buffer[3] << 8) | buffer[2];
        *g = (buffer[5] << 8) | buffer[4];
        *b = (buffer[7] << 8) | buffer[6];
    }
}