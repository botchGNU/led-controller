# Control LEDs via ATMega16

This project was created in order to control the brightness of an LED through the ATMega16 uC. This application is divided into 3 different modes:

- LED brightness controller
- SOS signal emitter
- Bicycle LED

## How does it work?

The main component of this application is the Timer1 functionality built into the ATMega16. This timer repeatetly counts up till his maximum value (1024) has been reached. If the current value matches the one of the OCR1A register, an interrupt is triggered. This signal is called a [phase correct PWM signal](https://atmega32-avr.com/phase-correct-pwm-pulse-width-modulation-mode-avr-microcontroller-timer/). The interrupt calls a special function called `invertLED()` which does exactly what it's name initiates in order to adjust the LEDs frequency. Another red LED can additionally be activated through a seperate button.

 ![](https://raw.githubusercontent.com/botchGNU/led-controller/main/Documentation/Images/abb1.gif)

The SOS-mode simply emmits a specific sos sequence:

long-long-long-short-short-short

## Button layout

| button | operation                   |
| ------ | --------------------------- |
| PA0    | decrease brighness          |
| PA1    | increase brighness          |
| PA2    | SOS-Mode                    |
| PA3    | Activate/deactivate red LED |

## Hardware

The application was created an tested on the ATMega16 board manufactured by Microchip. The required hardware components have already been soldered onto this board.

Documentation about this board can be [found here](https://github.com/botchGNU/megav4-projects/tree/main/megav4documentation)

## Hardware Structure

![](https://raw.githubusercontent.com/botchGNU/led-controller/main/Documentation/Images/abb2.png)



## Software Structure

The code was developed in the c-language through Microchip Studio. In order to flash the binaries onto the MEGACARD a special program called HID-Bootflash was used.

![abb3](https://raw.githubusercontent.com/botchGNU/led-controller/main/Documentation/Images/abb3.png)

### Libraries

- "interrupt.h": Interrupt library used by Timer1
- "delay.h": Delay library in order to create breaks between morse signals

````c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
````

### Interrupt service routine

ISR called by Timer1 on compare match

````c
ISR(TIMER1_COMPA_vect) //display signal on onboard LEDs
{
	invertLEDs(); //call invertled method
}
````

### Method : timersetup

Called once by main method in order to determine Timer1 settings

````c
void timerSetup(){
	
	OCR1A = 512;                         // set dutycycle to 50% (1024/2)

	TCCR1A |= (1<<COM1A0);					 // Set Timer1 in Phase Correct Mode 10 Bit
										 
	TCCR1A |= ((1<<WGM10)|(1<<WGM11));       // phase correct Mode 
											 //+ compare-Match OCR1A
	TCCR1B |= (1<<CS10);					 // prescaler 1
	
	TIMSK |= (1<<OCIE1A);  	 // Enable Timer 1 Overflow
						    // and Compare interrupt
	
}
````

### Method : invertLED

Method to check current LED condition and disable/enable them

````c
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
````

### Method : main

The first part of this method is executed only once in order to set up the ports and their directions

````c
	DDRC = 0xFF;  //LEDs output
	DDRA = 0x00;  //Set button ports 
	PORTC = 0x00; //disable LEDs
    PORTA = 0x0F;
	
	cli(); //disable interrupts
	timerSetup(); //execute timer-setup method
	sei(); //enable interrupts
````

#### main-loop

Every possible button combination gets checked through polling and the specific operations are executed afterwards.

````c
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
````



## Bug reports and contribution

If you want to contribute to this project or encounter a bug, feel free to  create a pull request or create a thread on the issues tab.

**Note:** Please add your system information when creating a bug report!

## Used Software

| Software                        | version  |
| ------------------------------- | -------- |
| Microchip Studio                | 7.0.2542 |
| Atmel Kits (Additional Package) | 7.0.132  |
| HID-Bootflash                   | 1.0      |

