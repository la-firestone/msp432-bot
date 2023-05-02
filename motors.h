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

#define MAX_TICKS   10000
#define DRIVE_SPEED 6000
#define TURN_SPEED 5000

// Function prototypes
void SysTick_Init();
void SysTick_Wait(uint32_t delay);
void delay1ms();
void delayms(int ms);
void motorInit();
void motorPWMInit();
void setMotorSpeed(int motor, int speed);
void setMotorDirection(int motor, int direction);
void stop();
void goForward();
void goForwardDelay(int delay);
void turnLeft();
void turnLeftDelay(int delay);
void turnRight();
void turnRightDelay(int delay);
void goReverse();
void goReverseDelay(int delay);

#endif /* MOTORS_H_ */
