// 2010 Kenneth Finnegan
// kennethfinnegan.blogspot.com


#define F_CPU		16000000L    // Crystal frequency in Hz

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "./libUART.h"

// CONSTANTS


// Output Compare 1 overflow interrupt
// Handles display multiplexing
SIGNAL (TIMER1_COMPA_vect)
{
	//PORTD = ~PORTD;
}

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0x20;

	// use CLK/1024 prescale value, clear timer/counter on compareA match
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12);
	// preset timer1 high/low byte
	OCR1A = ((F_CPU/1024) - 1 );   
	// enable Output Compare 1 overflow interrupt
	//TIMSK  = (1<<OCIE1A);
	// Enable interrupts
	sei();

	UART_init(4800);
	UART_putchar('W');
	UART_putchar('\n');
	UART_putchar('\r');

	UART_puts("Hello World!\n\r");
	UART_puts("This is a longer message!\n\rLet's hope it works well enough...\n\r");
	UART_puts("Hello World! 1\n\r");
	UART_puts("Hello World! 2\n\r");
	UART_puts("Hello World! 3\n\r");
	unsigned int i = 0;
	while (1) {
		i++;
		PORTB ^= 0x20;
		char charbuf[100];
		sprintf(charbuf, "Loop number %d\n\r",i); 
		UART_puts(charbuf);
	}
}

