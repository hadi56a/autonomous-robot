/**
 * @file motor.h
 * @brief Contrôle des moteurs du robot
 * @details Ce module permet de contrôler la vitesse et la direction des moteurs gauche et droit, ainsi que de freiner ou stopper le robot
 */

#ifndef MOTOR_H
#define MOTOR_H

/** @brief Pin PWM du moteur 1 (gauche/droit selon câblage) */
#define PIN_VM1 26 // 12
/** @brief Pin PWM du moteur 2 (gauche/droit selon câblage) */
#define PIN_VM2 23 // 13
/** @brief Pin de direction 1 pour moteur 1 */
#define PIN_IN1 4 // 23
/** @brief Pin de direction 2 pour moteur 1 */
#define PIN_IN2 5 // 24
/** @brief Pin de direction 1 pour moteur 2 */
#define PIN_IN3 2 // 27
/** @brief Pin de direction 2 pour moteur 2 */
#define PIN_IN4 0 // 17

/**
 * @brief Initialise les moteurs
 * @details Configure les broches en mode PWM ou sortie et initialise le mode PWM
 * @return 0 en cas de succès, -1 si WiringPi échoue
 */
int motor_init();

/**
 * @brief Fait avancer le robot à une vitesse donnée
 * @param speed La vitesse (0-100%)
 */
void motor_forward(int speed);

/**
 * @brief Tourne à gauche (moteur droit tourne, moteur gauche arrêté)
 * @param speed La vitesse (0-100%)
 */
void motor_turn_left(int speed);

/**
 * @brief Tourne à droite (moteur gauche tourne, moteur droit arrêté)
 * @param speed La vitesse (0-100%)
 */
void motor_turn_right(int speed);

/**
 * @brief Arrête les moteurs
 * @details Met toutes les entrées de contrôle à LOW et le PWM à 0
 */
void motor_stop();

/**
 * @brief Définit la vitesse du moteur gauche
 * @param speed La vitesse (positive pour avancer, négative pour reculer)
 */
void motor_set_left_speed(int speed);

/**
 * @brief Définit la vitesse du moteur droit
 * @param speed La vitesse (positive pour avancer, négative pour reculer)
 */
void motor_set_right_speed(int speed);

/**
 * @brief Freine le robot
 * @details Inverse la rotation pendant une courte durée pour un arrêt rapide, puis arrête les moteurs
 * @param speed La vitesse utilisée pour le freinage
 */
void motor_brake(int speed);

#endif