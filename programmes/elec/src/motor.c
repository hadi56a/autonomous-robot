/**
 * @file motor.c
 * @brief Implémentation des fonctions de commande des moteurs
 */

#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "motor.h"

int motor_init()
{

    if (wiringPiSetup() == -1) {
        return -1;
    }

	pinMode(PIN_VM1, PWM_OUTPUT);
	pinMode(PIN_VM2, PWM_OUTPUT);
	pinMode(PIN_IN1, OUTPUT);
	pinMode(PIN_IN2, OUTPUT);
	pinMode(PIN_IN3, OUTPUT);
	pinMode(PIN_IN4, OUTPUT);
	pwmSetMode(PWM_MODE_MS);

	return 0;
}

void motor_set_left_speed(int speed)
{
    if (speed >= 0) {
        digitalWrite(PIN_IN3, HIGH);
        digitalWrite(PIN_IN4, LOW);
        pwmWrite(PIN_VM2, speed * 10.24);
    } else {
        digitalWrite(PIN_IN3, LOW);
        digitalWrite(PIN_IN4, HIGH);
        pwmWrite(PIN_VM2, -speed * 10.24);
    }
}

void motor_set_right_speed(int speed)
{
    if (speed >= 0) {
		speed = speed * 0.95;
        digitalWrite(PIN_IN1, HIGH);
        digitalWrite(PIN_IN2, LOW);
        pwmWrite(PIN_VM1, speed * 10.24);
    } else {
        digitalWrite(PIN_IN1, LOW);
        digitalWrite(PIN_IN2, HIGH);
        pwmWrite(PIN_VM1, -speed * 10.24);
    }
}

void motor_forward(int speed)
{
	motor_set_left_speed(speed);
	motor_set_right_speed(speed);
}

void motor_turn_left(int speed) {
	motor_set_left_speed(0);
	motor_set_right_speed(speed);
}

void motor_turn_right(int speed) {
	motor_set_left_speed(speed);
	motor_set_right_speed(0);
}

void motor_stop() {
	digitalWrite(PIN_IN1, LOW);
	digitalWrite(PIN_IN2, LOW);
	pwmWrite(PIN_VM1, 0);
	
	digitalWrite(PIN_IN3, LOW);
	digitalWrite(PIN_IN4, LOW);
	pwmWrite(PIN_VM2, 0);
}

void motor_brake(int speed) {
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, HIGH);
    digitalWrite(PIN_IN1, LOW); 
    digitalWrite(PIN_IN2, HIGH);
    
    pwmWrite(PIN_VM2, speed * 10.24);
    pwmWrite(PIN_VM1, speed * 10.24);
    
    usleep(50000);
    motor_stop();
}