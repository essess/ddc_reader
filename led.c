#include <system.h>
#include "types.h"
#include "led.h"

#ifdef _ICD2
#include <icd2.h>
#else
#endif // _ICD2

#pragma message "public functions available from this module are prefixed w/'led'"

volatile bit flagIsLedInitialized  = false;

ledInfo_t	greenStateInfo;
ledInfo_t	redStateInfo;

volatile uint16_t  onCntRedLED;					// hold count of 'persist time' in ms, int handler will dec this each round automagically for us
volatile uint16_t  offCntRedLED;
volatile uint16_t  onCntGreenLED;
volatile uint16_t  offCntGreenLED;

const 	 bit LEDOn  = 1;			
const 	 bit LEDOff = !LEDOn;

volatile bit redLED@PORTB.1;	 
volatile bit greenLED@PORTB.2; 

volatile bit trisRedLED@TRISB.1;
volatile bit trisGreenLED@TRISB.2;

inline void redOn(void) { trisRedLED = Output; redLED = LEDOn; }
inline void redOff(void) { trisRedLED = Output; redLED = LEDOff; }
inline void greenOn(void) { trisGreenLED = Output; greenLED = LEDOn; }
inline void greenOff(void) { trisGreenLED = Output; greenLED = LEDOff; }

//void ledSetState(void) {}

void ledSetState(const ledInfo_t * const li) {
	if(!flagIsLedInitialized) ledInit();
	
	if(li->color == RED) {
		if(li->action == ON) {  // only support on/off for now
			redOn();
			onCntRedLED = li->onDuration;
			redStateInfo.onDuration = onCntRedLED;
			redStateInfo.action = ON;
			}
		if(li->action == OFF) {
			redOff();
			onCntRedLED = 0x0;
			redStateInfo.onDuration  = 0x0;
			redStateInfo.offDuration = 0x0;
			redStateInfo.action = OFF;
			}
		return;
		}
			
	if(li->color == GREEN) {
		if(li->action == ON) {  // only support on/off for now
			greenOn();
			onCntGreenLED = li->onDuration;
			greenStateInfo.onDuration = onCntGreenLED;
			greenStateInfo.action = ON;
			}
		if(li->action == OFF) {
			greenOff();
			onCntGreenLED = 0x0;
			greenStateInfo.onDuration  = 0x0;
			greenStateInfo.offDuration = 0x0;
			greenStateInfo.action = OFF;
			}
		return;
		}			
		
}

void ledGetState(ledInfo_t * const li) {
	if(!flagIsLedInitialized) ledInit();
	
	if(li->color == RED) {
		li->action = redStateInfo.action;	// it's possible to have persist counter expire and automatically turn off the led for us
		li->onDuration = onCntRedLED;		// counters are constantly being updated and in this function we're checking status
		li->offDuration = offCntRedLED;
		return;
		}
		
	if(li->color == GREEN) {
		li->action = greenStateInfo.action;
		li->onDuration = onCntGreenLED;
		li->offDuration = offCntGreenLED;
		return;
		}
		
}

void ledInit(void) {
	
	redOff();	
	redStateInfo.color       = RED;
	redStateInfo.action      = OFF;
	redStateInfo.onDuration  = 0x0;
	redStateInfo.offDuration = 0x0;
	
	greenOff();
	greenStateInfo.color       = GREEN;
	greenStateInfo.action      = OFF;
	greenStateInfo.onDuration  = 0x0;
	greenStateInfo.offDuration = 0x0;
	
	flagIsLedInitialized = true;
	
}

void led1MSUpdate(void) {
	
	if(redStateInfo.onDuration) {  	// if non zero then we have to update our internal persist counters
		onCntRedLED ? onCntRedLED-- : redOff();
		}
		
	if(greenStateInfo.onDuration) { // if non zero then we have to update our internal persist counters
		onCntGreenLED ? onCntGreenLED-- : greenOff();
		}

}

