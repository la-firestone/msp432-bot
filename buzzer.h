/*
 * buzzer.h
 *
 *  Created on: May 1, 2023
 *      Author: lafirestone
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <msp.h>

#define C5   523
#define CS5  554
#define D5   587
#define DS5  622
#define E5   659
#define F5   698
#define FS5  740
#define G5   784
#define GS5  831
#define A5   880
#define AS5  932
#define B5   988
#define C6   1047

// Function prototypes
void buzzerInit();
void playNote(int frequency);
void mute();

#endif /* BUZZER_H_ */
