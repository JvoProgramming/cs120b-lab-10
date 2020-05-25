/*	Author: Sabrina Carlos
 *  Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include<avr/interrupt.h>
#include "timer.h"



   unsigned char buzzybound = 5;
   unsigned char threeLEDs = 0x00;
   unsigned char blinkingLED = 0x00;
   unsigned char tmpB = 0x00;
   unsigned char tmpA = 0x00;
   unsigned char buzz = 0x00;

enum TG_states{ begin, wait, inc, inc_wait, dec, dec_wait} TG_state;
void ToggleSM(){ // combine state variables and output to PORTB
	switch(TG_state){
		case begin: 
			TG_state = wait;
			break;
		case wait: 
			if((tmpA & 0x01) == 0x01){
				TG_state = inc;
			}
			else if((tmpA & 0x02) == 0x02){
				TG_state = dec;
			}
			else{
				TG_state = wait;
			}
			break;
		case inc: 
			TG_state = inc_wait;
			break;
		case inc_wait: 
			if((tmpA & 0x01) == 0x01){
				TG_state = inc_wait;
			}
			else{
				TG_state = wait;
			}
			break;
		case dec: 
			TG_state = dec_wait;
			break;
		case dec_wait: 
			if((tmpA & 0x02) == 0x02){
				TG_state = dec_wait;
			}
			else{
				TG_state = wait;
			}
			break;
	}
	switch(TG_state){
		case begin: 
			buzzybound = 1;
			break;
		case wait: 
			buzzybound = buzzybound;
			break;
		case inc: 
			if (buzzybound > 1) { buzzybound--;}
			break;
		case inc_wait: 
			buzzybound = buzzybound;
			break;
		case dec: 
			buzzybound++; //higher bound? 
			break;
		case dec_wait: 
			buzzybound = buzzybound;
			break;
	}
}

enum TL_states { TL_SMStart, TL_T0, TL_T1, TL_T2 } TL_state;
void ThreeLEDsSM() {
	switch(TL_state){
		case TL_SMStart:
			TL_state = TL_T0;
			break;
		case TL_T0:
			TL_state = TL_T1;
			break;
		case TL_T1:
			TL_state = TL_T2;
			break;
		case TL_T2:
			TL_state = TL_T0;
			break;
	}
	switch(TL_state){
		case TL_SMStart:
			//threeLEDs = 0x00;
			break;
		case TL_T0:
			threeLEDs = 0x01;
			break;
		case TL_T1:
			threeLEDs = 0x02;
			break;
		case TL_T2:
			threeLEDs = 0x04;
			break;
	}
}

enum BL_states { BL_SMStart, BL_LedOff, BL_LedOn } BL_state;
void BlinkingLEDSM() {
	switch(BL_state){
		case BL_SMStart:
			BL_state = BL_LedOn;
			break;
		case BL_LedOff:
			BL_state = BL_LedOn;
			break;
		case BL_LedOn:
			BL_state = BL_LedOff;
			break;
	}
	switch(BL_state){
		case BL_SMStart:
			//blinkingLED = 0x00;
			break;
		case BL_LedOff:
			blinkingLED = 0x00;
			break;
		case BL_LedOn:
			blinkingLED = 0x08;
			break;
	}
}

enum CL_states{ starty, or} CL_state;
void CombineLEDsSM(){ // combine state variables and output to PORTB
	switch(CL_state){
		case starty: 
			CL_state = or;
			break;
		case or: 
			CL_state = or;
			break;
	}
	switch(CL_state){
		case starty: 
			tmpB = 0x00;
			break;
		case or:
			tmpB = (threeLEDs|blinkingLED|buzz);
			break;
	}
}
enum Buzz_states{ startz, H, L} Buzz_state;
void BuzzerSM(){ // combine state variables and output to PORTB
	switch(Buzz_state){
		case startz: 
			Buzz_state = H;
			break;
		case H: 
			Buzz_state = L;
			break;
		case L: 
			Buzz_state = H;
			break;
	}
	switch(Buzz_state){
		case startz: 
			buzz = 0x00;
			break;
		case H: 
			buzz = 0x10;
			break;
		case L: 
			buzz = 0x00;
			break;
	}
}


void main() {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
   unsigned long BL_elapsedTime = 1000;
   unsigned long TL_elapsedTime = 300;
   unsigned long Buzz_elapsedTime = 2;
   unsigned long CL_elapsedTime = 1;
   unsigned long TG_elapsedTime = 1;
   const unsigned long timerPeriod = 1;

   TimerSet(timerPeriod);
   TimerOn();
   tmpA = 0x00;
   TG_state = begin;
   BL_state = BL_SMStart;
   TL_state = TL_SMStart;
   CL_state = or;
   while (1) {
	tmpA = ~PINA;
      if (TG_elapsedTime >= 1) {
	ToggleSM();
	TG_elapsedTime = 0;
      }
      if (BL_elapsedTime >= 1000) { // 1000 ms period
         BlinkingLEDSM(); // Execute one tick of the BlinkLed synchSM
         BL_elapsedTime = 0;           
      }
      if (TL_elapsedTime >= 300) { // 300 ms period
         ThreeLEDsSM(); // Execute one tick of the ThreeLeds synchSM
         TL_elapsedTime = 0;
      }      
      if (CL_elapsedTime >= 1) {
	CombineLEDsSM();
	CL_elapsedTime = 0;
      }
	if(tmpA & 0x04){
	      if (Buzz_elapsedTime >= buzzybound) { // variable ms period
		 BuzzerSM(); // Execute one tick of the ThreeLeds synchSM
		 Buzz_elapsedTime = 0;
	      }
		//tmpB = tmpB | buzz;
	}
	else{
		buzz = 0x00;
		Buzz_elapsedTime = 0;
	}

	PORTB = tmpB;
      while (!TimerFlag){}   // Wait for timer period
      TimerFlag = 0;         // Lower flag raised by timer
      BL_elapsedTime += timerPeriod;
      TL_elapsedTime += timerPeriod;
	CL_elapsedTime += timerPeriod;
	TG_elapsedTime += timerPeriod;
	Buzz_elapsedTime += timerPeriod;
   }
}

