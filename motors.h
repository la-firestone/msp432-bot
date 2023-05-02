/*
 * motors.h
 *
 *  Created on: Apr 29, 2023
 *      Author: lafirestone
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include <msp.h>
#include <stdint.h>
#include <stdio.h>
#include "lcdLib_432.h"

// Pin configuration

#define ENA_PIN BIT5 //P2.5
#define ENB_PIN BIT4 //P2.4
#define IN1_PIN BIT0 //P3.0
#define IN2_PIN BIT6 //P3.6
#define IN3_PIN BIT7 //P3.7
#define IN4_PIN BIT5 //P3.5


// Function prototypes
void SysTick_Init();
void SysTick_Wait(uint32_t delay);
void delay1ms();
void delayms(int ms);
void setupGPIO();
void setupPWM();
void setMotorSpeed(int motor, int speed);
void setMotorDirection(int motor, int direction);
void stop();
void goForward(int delay);
void turnLeft(int delay);
void turnRight(int delay);
void goReverse(int delay);

#endif /* MOTORS_H_ */
