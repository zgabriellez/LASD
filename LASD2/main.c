/*
 * LASD2.c
 * Created: 06/03/2021 11:46:37
 * Author : Gabrielle Pereira Barbosa
 * Matricula 118110183
 */ 
#include "defs.h"
#include "nokia5110.h"


int main(void)
{
	int a = 1;
	enabling(MYUBRR);
	DDRB = 0b00000110; //todos os pinos B como saídas
	DDRD = 0b10000000; //todos os pinos D exceto D7 como entradas
	DDRC = 0b00000000; //todos os pinos C como entradas
	
	PORTD = 0b01111111; //habilita resistores pullup
	PORTC = 0b01111111;
	PORTB = 0b10000000;
	
	ICR1 = 39999;
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011010;
		
	OCR1A = 2000;
	
	EICRA = 0b00001010; //interrupçao INT0 e INT1
	EIMSK = 0b00000011;
	
	//habilitacao do ADC
	
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	
	//interrupçao por comparacao (de 1 ms)
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011; //habilita prescaler
	OCR0A = 249;
	TIMSK0 = 0b00000010;
	
	//interrupção PCINT16
	PCICR = 0b00000111; //Habilita interrupção dos pinos D, C e B (PCINT)
	PCIFR = 0b00000111;
	PCMSK0 = 0b10000000;
	PCMSK1 = 0b01000000; //habilita interrupcao no pino 6C
	PCMSK2 = 0b00010000; //Habilita interrupção do pino  21
	
	UCSR0B |= (1<<RXCIE0); //interrupt usart
	
	sei();
	nokia_lcd_init();

	while (1)
	{
		funcao_led(&a);
	}
}
