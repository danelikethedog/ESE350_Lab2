#include <avr/io.h> 
#include <avr/interrupt.h>
#include "util/delay.h" 
#include <stdio.h>
#include "uart.h"

unsigned int pinbRead, left_edge, right_edge, overflows;
unsigned long pulse_width, dif;

ISR(TIMER1_CAPT_vect) {
	
	pinbRead = PINB & 0x01;

	overflows = 0;

	//set the timer overflow interrupt and get left edge
	TIMSK1 |= 0x21;
	left_edge = ICR1;

	//while loop while key is depressed w/ led on
	while(pinbRead == 1) {
		pinbRead = PINB & 0x01;
		if (pinbRead == 1) {
			PORTB |= 0x30;
		} else {
			PORTB &= ~(1 << 5);
			PORTB &= ~(1 << 4);
		}
	}


	right_edge = ICR1;
	if (right_edge < left_edge) {
		overflows--;
	}

	dif = right_edge - left_edge;
	pulse_width = (long)overflows * 65536 + dif;
	
}

ISR(TIMER1_OVF_vect) {
	overflows++;
}


int main(void)
{
	overflows = 0;
	left_edge = 0;
	right_edge = 0;
	pulse_width = 0;
	dif = 0;

	DDRB = 0xFE;

	PORTB = 0x00;

	TCCR1B |= 0x42;

	TIMSK1 = 0x20;

	sei();

	uart_init();

	while(1) {

		printf(" %i \n", pulse_width);

		if (pulse_width > 60000 && pulse_width < 400000) {
			PORTB |= 0x10;
			uart_putchar('.', &uart_out);
			overflows = 0;
		} else if (pulse_width > 400000 && pulse_width < 800000) {
			uart_putchar('-', &uart_out);
			overflows = 0;
		} else if (pulse_width > 800000) {
			uart_putchar(' ', &uart_out);
			overflows = 0;
		} 
	}


}