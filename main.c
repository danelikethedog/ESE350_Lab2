#include <avr/io.h> 
#include <avr/interrupt.h>
#include "util/delay.h" 
#include <stdio.h>
#include "uart.h"

unsigned int pinbRead, dif, left_edge, right_edge, overflows;
unsigned long pulse_width;

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
			PORTB |= 0x20;
		} else {
			PORTB &= 0x00;
		}
	}


	right_edge = ICR1;
	if (right_edge < left_edge) {
		overflows--;
	}

	dif = right_edge - left_edge;
	pulse_width = (long)overflows * 0.00002133 + (long)dif;
	
}

ISR(TIMER1_OVF_vect) {
	overflows++;
}


int main(void)
{
	overflows = 0;
	left_edge = 0;
	pulse_width = 0;

	DDRB = 0xFE;

	PORTB = 0x10;

	TCCR1B |= 0x41;

	TIMSK1 = 0x20;

	sei();

	uart_init();

	uart_putchar('s', &uart_out);

	while(1) {

		if (pulse_width > 1407 && pulse_width < 9376) {
			uart_putchar('.', &uart_out);
		} else if (pulse_width > 9376 && pulse_width < 18752) {
			uart_putchar('-', &uart_out);
		} else if (pulse_width > 18752) {
			uart_putchar(' ', &uart_out);
		}
	}


}