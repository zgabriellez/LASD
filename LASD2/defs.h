/*
 * defs.h
 *
 * Created: 20/05/2021 21:28:16
 *  Author: Gabi
 */ 


#ifndef DEFS_H_
#define DEFS_H_

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1 //modo assinc
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "nokia5110.h"


void enabling (unsigned int ubrr);

unsigned char USART_Recepcao();

void USART_Transmit(unsigned char data);

void limita ();

void limitaO2(uint8_t *O);

double timing_google ();

void pressure();

void conversao ();

void buzzer ();

void funcao_led (int *a);

ISR(TIMER0_COMPA_vect);

void func_servo();

ISR(PCINT2_vect);

ISR(INT0_vect);

ISR(INT1_vect);

ISR(ADC_vect);

ISR(PCINT1_vect);

ISR(PCINT0_vect);

ISR(USART_RX_vect);


#endif /* DEFS_H_ */