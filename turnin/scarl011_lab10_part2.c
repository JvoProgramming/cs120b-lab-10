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



   unsigned char threeLEDs = 0x00;
   unsigned char blinkingLED = 0x00;
   unsigned char tmpB = 0x00;

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
			tmpB = (threeLEDs|blinkingLED);
			break;
	}
}

void main() {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
   unsigned long BL_elapsedTime = 1000;
   unsigned long TL_elapsedTime = 300;
   unsigned long CL_elapsedTime = 1;
   const unsigned long timerPeriod = 50;

   TimerSet(timerPeriod);
   TimerOn();
   BL_state = BL_SMStart;
   TL_state = TL_SMStart;
   CL_state = or;
   while (1) {
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
	PORTB = tmpB;
      while (!TimerFlag){}   // Wait for timer period
      TimerFlag = 0;         // Lower flag raised by timer
      BL_elapsedTime += timerPeriod;
      TL_elapsedTime += timerPeriod;
	CL_elapsedTime += timerPeriod;
   }
}

