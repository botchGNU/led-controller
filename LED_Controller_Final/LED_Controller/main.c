/*
 * CTC_LED.c
 *
 * Created: 12/21/2021 2:51:03 PM
 * Author : Kicker Tim
 * Usage  : Control duty cicle by buttons PA0 & PA1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

unsigned char redLight = 0; //red led' disbaled by default

ISR(TIMER1_COMPA_vect) //display signal on onboard LEDs
{
	invertLEDs(); //call invertled method
}

void timerSetup(){
	
	OCR1A = 512;                         // set dutycycle to 50% (1024/2)

	TCCR1A |= (1<<COM1A0);					 // Set Timer1 in Phase Correct Mode 10 Bit
										 
	TCCR1A |= ((1<<WGM10)|(1<<WGM11));       // phase correct Mode 
											 //+ compare-Match OCR1A
	TCCR1B |= (1<<CS10);					 // prescaler 1
	
	TIMSK |= (1<<OCIE1A);  	 // Enable Timer 1 Overflow
						    // and Compare interrupt
	
}

void invertLEDs(){
	if ((PORTC == 0xFF ) | (PORTC == 0xFC)){ //are leds currently enabled?
		PORTC = 0x00; //disable LED's
	}
	else {
		if (redLight == 1){ //red LED activated?
			PORTC = 0xFF; //enable all LEDs
		}
		else {PORTC = 0xFC; } //control only white LED
	}
}

int main(void)
{
	DDRC = 0xFF;	//LEDs output
	DDRA = 0x00;   //Set button ports 
	PORTC = 0x00; //disable LEDs
    PORTA = 0x0F;
	
	cli(); //disable interrupts
	timerSetup(); //execute timer-setup method
	sei(); //enable interrupts
	
    while (1) { 

		
		if((OCR1A < 1020) && (!(PINA&(1<<PA0)))){ //Button 0 pressed? & has not reached max. value?
			OCR1A++; //increment duty-cycle
			_delay_ms(10);
		}
		else if( (OCR1A > 5) && (!(PINA&(1<<PA1)))){ //Button 1 pressed? & has not reached min. value?
			OCR1A--; //decrement duty-cycle
			_delay_ms(10);
		}
		else if (!(PINA&(1<<PA3))){ //activate/deactivate red LED
			if (redLight == 0){redLight = 1;}
				else{redLight = 0;}
			_delay_ms(40); //delay in order to prevent
		}
		else if ((!(PINA &(1<<PA0))) && (!(PINA &(1<<PA1)))){
				
		}
		 
		 else if( (!(PINA &(1<<PA2)))){ //SOS-mode
			OCR1A =6;
			while(1)
			{
				PORTC=0xFF; // long
				_delay_ms(5000);
				PORTC=0x00; // pause
				_delay_ms(5000);
				PORTC=0xFF; // long
				_delay_ms(5000);
				PORTC=0x00; // pause
				_delay_ms(5000);
				PORTC=0xFF; // long
				_delay_ms(5000);
				PORTC=0x00; // pause
				_delay_ms(5000);
				PORTC=0xFF; // Kurz
				_delay_ms(1000);
				PORTC=0x00; // pause
				_delay_ms(1000);
				PORTC=0xFF; // Kurz
				_delay_ms(1000);
				PORTC=0x00; // pause
				_delay_ms(1000);
				PORTC=0xFF; // Kurz
				_delay_ms(1000);
				PORTC=0x00; // pause (long)
				_delay_ms(5000);
			}

		 }
    }
}