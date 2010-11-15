// 2010 Kenneth Finnegan
// kennethfinnegan.blogspot.com
//

#define F_CPU		16000000L    // Crystal frequency in Hz

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "libUART.h"

// CONSTANTS


// Output Compare 1 overflow interrupt
// Handles display multiplexing
SIGNAL (TIMER1_COMPA_vect)
{
	PORTD = ~PORTD;
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

	UART_Init();

	UART_tx("Hello World!\n");
	UART_tx("This is a longer message!\nLet's hope it works well enough...\n");
	UART_tx("Hello World! 1\n");
	UART_tx("Hello World! 2\n");
	UART_tx("Hello World! 3\n");
	while (1) {
		_delay_ms(1000);
		PORTB ^= 0x20;
	}
}
