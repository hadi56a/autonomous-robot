/**
 * @file buzzer.h
 * @brief Gestion du buzzer pour le robot
 * @details Ce module permet de contrôler un buzzer connecté via WiringPi
 */

#ifndef BUZZER_H
#define BUZZER_H

/**
 * @brief Pin WiringPi du buzzer
 * @details WiringPi pin 1 correspond au GPIO18 sur le Raspberry Pi
 */
#define BUZZER_PIN 1

/**
 * @brief Initialise le buzzer
 * @details Configure la broche du buzzer en mode sortie
 * Cette fonction doit être appelée avant toute utilisation du buzzer
 */
void buzzer_setup(void);

/**
 * @brief Fait bipper le buzzer pendant une durée spécifiée, attention : appel bloquant
 * @param duration_ms La durée du bip en millisecondes
 */
void buzzer_beep(int duration_ms);

#endif
