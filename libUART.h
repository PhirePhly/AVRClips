// Init and run the UART subsystem on AVRs
// 2010 Kenneth Finnegan - GPLv2
// kennethfinnegan.blogspot.com
//

// Uses a circular buffer to cache transmitted and received bytes
// Typical usage:
//    UART_init();
//    UART_puts("Debug message");
//    char stringbuf[100];
//    sprintf(stringbuf, "Error num %d\n", 10);
//    UART_puts(stringbuf);
//
// TODO:
//	* Implement receive
//	* Allow any baud rate


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


// CONSTANTS
#define UART_BAUD	300
#define UARTBUFSIZE 32

#define UART_UBRR  (F_CPU/16/UART_BAUD - 1)

// GLOBALS

struct _uart {
	uint8_t txposition;
	volatile uint8_t txlength;
	char txbuf[UARTBUFSIZE];
};
volatile struct _uart uart;

void UART_init(unsigned int baud)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(UART_UBRR >> 8);
	UBRR0L = (unsigned char)(UART_UBRR & 8);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	uart.txposition = 0;
	uart.txlength = 0;
}

void UART_putchar(char c) {
	// Spinlock on full buffer
	while (uart.txlength >= UARTBUFSIZE-1) ; 
	// Add character to buffer
	uart.txbuf[(uart.txposition+uart.txlength)%UARTBUFSIZE] = c;
	uart.txlength = (uart.txlength + 1) % UARTBUFSIZE;
	// Enable empty tx register vector
	UCSR0B |= (1<<UDRIE0);
}

void UART_puts(char *string) {
	if (*string == '\0') return;
	do {
		UART_putchar(*string);
	} while (*(++string)); // Until '\0'
}

// Tx - Data register empty
SIGNAL (USART_UDRE_vect) {
	UDR0 = uart.txbuf[uart.txposition];
	uart.txposition = (uart.txposition + 1) % UARTBUFSIZE;
	uart.txlength--;
	if (uart.txlength == 0) {
		//Disable interupt
		UCSR0B &= ~(1<<UDRIE0);
	}
	return;
}

// Tx complete
SIGNAL (USART_TX_vect) {

}

