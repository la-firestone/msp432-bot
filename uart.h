/*
 * uart.h
 *
 *  Created on: May 3, 2023
 *      Author: lafirestone
 */

#ifndef UART_H_
#define UART_H_

#include <msp.h>
#include <stdio.h>

void UARTInit();
void UARTsendString(char *str);

#endif /* UART_H_ */
