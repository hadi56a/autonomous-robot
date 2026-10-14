/**
 * @file line.h
 * @brief Gestion des capteurs de ligne (suiveur de ligne)
 * @details Ce module permet d'initialiser et de lire les états des capteurs infrarouges de détection de ligne
 */

#ifndef LINE_H
#define LINE_H

#include <stdbool.h>

/** @brief Pin du capteur arrière gauche (GPIO21) */
#define PIN_BACK_LEFT 21  
/** @brief Pin du capteur avant gauche (GPIO22) */
#define PIN_FRONT_LEFT 22      
/** @brief Pin du capteur avant droit (GPIO3) */
#define PIN_FRONT_RIGHT 3   
/** @brief Pin du capteur arrière droit (GPIO27) */
#define PIN_BACK_RIGHT 27
/** @brief Pin du capteur avant central (GPIO6) */
#define PIN_FRONT_CENTER 6

/**
 * @brief Initialise les capteurs de ligne
 * @return 0 en cas de succès, -1 en cas d'erreur lors de l'initialisation de WiringPi
 */
int line_init();

/**
 * @brief Lit l'état du capteur avant gauche
 * @return true si une ligne est détectée (HIGH), false sinon
 */
bool line_front_left();

/**
 * @brief Lit l'état du capteur avant droit
 * @return true si une ligne est détectée (HIGH), false sinon
 */
bool line_front_right();

/**
 * @brief Lit l'état du capteur avant central
 * @return true si une ligne est détectée (HIGH), false sinon
 */
bool line_front_center();

/**
 * @brief Lit l'état du capteur arrière gauche
 * @return true si une ligne est détectée (HIGH), false sinon
 */
bool line_back_left();

/**
 * @brief Lit l'état du capteur arrière droit
 * @return true si une ligne est détectée (HIGH), false sinon
 */
bool line_back_right();

#endif