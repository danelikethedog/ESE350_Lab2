#include <avr/io.h> 
#include <avr/interrupt.h>
#include "util/delay.h" 
#include <stdio.h>
#include "uart.h"

unsigned int pinbRead, left_edge, right_edge, overflows, pulse_width;
unsigned long dif;

ISR(TIMER1_CAPT_vect) {
	PORTB ^= 0x20;
}

ISR(TIMER1_OVF_vect) {
	overflows++;
}


int main(void)
{
	

	DDRB = 0xFE;

	PORTB = 0x18;

	TCCR1B |= 0x41;

	TIMSK1 |= 0x20;

	uart_init();

	sei();

	while(1) {

		//printf("Pulse Width %i\n", pulse_width);
		//printf("Overflows %i\n", overflows);

		overflows = 0;
		left_edge = 0;
		right_edge = 0;
		pulse_width = 0;
		dif = 0;

		TCCR1B |= 0x40;

		
		
		//Turn on the overflow interupt
		
		//Input High Sensitive
		TIFR1 |= 0x20;

		//wait for input flag to be tripped
		while(!(TIFR1 & 0x20));	

		PORTB |= 0x18;

		//turn on overflows
		TIMSK1 |= 0x21;

		//Hold while button is pressed down
		pinbRead = PINB & 0x01;
		while(pinbRead == 1) {
 			pinbRead = PINB & 0x01;
 			if(pinbRead == 1) {
	 			PORTB |= 0x20;
	 		} else {
	 			PORTB &= ~(1 << 5);
	 		}
 		}


		//Turn Off 7 Segment Lines
		TIMSK1 &= ~(1 << 0);
		TIMSK1 &= ~(1 << 5);


		pulse_width = overflows * 4;

		//Determine kind of incoming and turn on 7 Segment display
		if (pulse_width > 30 && pulse_width < 200) {
			uart_putchar('.', &uart_out);
			PORTB &= ~(1 << 4);
		} else if (pulse_width >= 200 && pulse_width < 399) {
			uart_putchar('-', &uart_out);
			PORTB &= ~(1 << 3);
		} else if (pulse_width >= 400) {
			uart_putchar(' ', &uart_out);
			PORTB |= 0x18;
		}
	 
	}


}