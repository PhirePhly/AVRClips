// 2010 Kenneth Finnegan
// kennethfinnegan.blogspot.com
//

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


// CONSTANTS
#define UART_BAUD	9600
#define UARTBUFSIZE 32

#define UART_UBRR  (F_CPU/16/UART_BAUD - 1)

// GLOBALS

struct _uart {
	uint8_t position;
	uint8_t length;
	char buf[UARTBUFSIZE];
};
volatile struct _uart uart;


// Tx - Data register empty
SIGNAL (USART_UDRE_vect) {
	UDR0 = uart.buf[uart.position];
	uart.position = (uart.position + 1) % UARTBUFSIZE;
	uart.length--;
	if (uart.length == 0) {
		//Disable interupt
		UCSR0B &= ~(1<<UDRIE0);
	}
	return;
}

// Tx complete
SIGNAL (USART_TX_vect) {

}

void UART_Init( )
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(UART_UBRR >> 8);
	UBRR0L = (unsigned char)(UART_UBRR & 8);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	uart.position = 0;
	uart.length = 0;
}

void UART_tx(char *string) {
	if (*string == '\0') return;
	do {
		// Spinlock on full buffer
		while (uart.length >= UARTBUFSIZE-1);
		// Add character to buffer
		uart.buf[(uart.position+uart.length)%UARTBUFSIZE] = *string;
		uart.length = (uart.length + 1) % UARTBUFSIZE;
		// Enable empty tx register vector
		UCSR0B |= (1<<UDRIE0);
	} while (*(++string)); // Until '\0'
}

