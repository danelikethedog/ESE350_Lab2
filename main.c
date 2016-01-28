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

char getChar() {

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
			//uart_putchar('.', &uart_out);
			PORTB &= ~(1 << 4);
			return '.';
		} else if (pulse_width >= 200 && pulse_width < 399) {
			//uart_putchar('-', &uart_out);
			PORTB &= ~(1 << 3);
			return '-';
		} else if (pulse_width >= 400) {
			//uart_putchar(' ', &uart_out);
			PORTB |= 0x18;
			return ' ';
		}



}

int main(void) {
	

	DDRB = 0xFE;

	PORTB = 0x18;

	TCCR1B |= 0x41;

	TIMSK1 |= 0x20;

	uart_init();

	sei();

	char newChar;

	while(1) {
		newChar = getChar();

		//Start with a Dot
		if(newChar == '.') {
			newChar = getChar();
			if(newChar == '.') { // ..
				newChar = getChar();
				if(newChar == '.') { // ...
					newChar = getChar();
					if(newChar == '-') { //...-
						newChar = getChar();
						if(newChar == ' ') {
							uart_putchar('V', &uart_out);
						} else if(newChar == '-') { //...--
							uart_putchar('3', &uart_out);
						} 
					} else if(newChar == '.') { //....
						newChar = getChar();
						if(newChar == ' ') {
							uart_putchar('H', &uart_out);
						} else if(newChar == '-') { //....-
							uart_putchar('4', &uart_out);
						} else if(newChar == '.') { //.....
							uart_putchar('5', &uart_out);
						}
					} else {
						uart_putchar('S', &uart_out);
					}
				} else if(newChar == '-') { //..-
					newChar = getChar();
					if(newChar == '.') { // ..-.
						uart_putchar('F', &uart_out);
						
					} else if(newChar == ' ')  {
						uart_putchar('U', &uart_out);
						
					} else { //..--
						newChar = getChar();
						if(newChar == '-') { //..---
							uart_putchar('2', &uart_out);
							
						}
					}
				} else { // ..
					uart_putchar('I', &uart_out);
				}
			 } else if(newChar == '-') { //.-
				newChar = getChar();
				if(newChar == '.') { //.-.
					newChar = getChar();
					if(newChar == '.') { // .-..
						uart_putchar('L',&uart_out);
					}
					else if (newChar == ' ') { // .-.
						uart_putchar('R', &uart_out); 
					}
				} else if(newChar == '-') { //.--
					newChar = getChar();
					if(newChar == '-') { //.---
						newChar = getChar();
						if(newChar == '-') { //.----
							uart_putchar('1', &uart_out);
						} else {
							uart_putchar('J', &uart_out);
							
						}
					} else if(newChar == '.') { // .--.
						uart_putchar('P',&uart_out);
						
					} else { 
						uart_putchar('W', &uart_out);
						
					}
				} else { //.-
					uart_putchar('A',&uart_out);
					
				}
			} else {
				uart_putchar('E', &uart_out);
				
			}

		//Start With a Dash
		} else if(newChar == '-') {
			newChar = getChar();
			if(newChar == '.') { //-.
				newChar = getChar();
				if(newChar == '.') { //-..
					newChar = getChar();
					if(newChar == '-') { //-..-
						uart_putchar('X', &uart_out);
						
					} else if(newChar == '.') { //-...
						newChar = getChar();
						if(newChar == ' ') {
							uart_putchar('B', &uart_out);
							
						} else if(newChar == '.') { //-....
							uart_putchar('6', &uart_out);
							
						}
					} else {
						uart_putchar('D', &uart_out);
						
					}
				} else if(newChar == '-') { //-.-
					newChar = getChar();
					if(newChar == '.') { //-.-.
						uart_putchar('C', &uart_out);
						
					} else if(newChar == '-') { //-.-- 
						uart_putchar('Y', &uart_out);
						
					} else { //-.-
						uart_putchar('K', &uart_out);
						
					}
				} else { //-.
					uart_putchar('N',&uart_out);
					
				}
			} else if(newChar == '-') { //--
				newChar = getChar();
				if(newChar == '-') { //---
					newChar = getChar();
					if(newChar == ' ') {
						uart_putchar('O', &uart_out);
						
					} else if(newChar == '.') { //---.
						newChar = getChar();
						if(newChar == '.') { //---..
							uart_putchar('8', &uart_out);
							
						}
					} else { //----
						newChar = getChar();
						if(newChar == '.') { //----.
							uart_putchar('9', &uart_out);
							
						} else if(newChar == '-') { //-----
							uart_putchar('0', &uart_out);
							
						} 
					}
				} else if(newChar == '.') { //--.
					newChar = getChar();
					if(newChar == '.') { //--..
						newChar = getChar();
						if(newChar == ' ') {
							uart_putchar('Z', &uart_out);
							
						} else if(newChar == '.') { //--...
							uart_putchar('7', &uart_out);
							
						}
					} else if(newChar == '-') { //--.-
						uart_putchar('Q', &uart_out);
						
					} else { //--.
						uart_putchar('G', &uart_out);
						
					}
				} else { //--
					uart_putchar('M',&uart_out);
					
				}
			} else { // -
				uart_putchar('T', &uart_out);
				
			}
		} else { // newChar == ' '
			uart_putchar(' ', &uart_out);
		}
	 
	}


}