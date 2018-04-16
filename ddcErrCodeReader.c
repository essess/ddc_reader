#include <system.h>
#include "types.h"
#include "ddcErrCodeReader.h"
#include "lcd.h"
#include "led.h"
#include "errText.h"
#include "cb.h"

#ifdef _ICD2
#include <icd2.h>
#pragma DATA _CONFIG, _PWRTE_ON & _BODEN_ON & _WDT_OFF & _HS_OSC & _DEBUG_ON & _LVP_OFF
#else 
#pragma DATA _CONFIG, _PWRTE_ON & _BODEN_ON & _WDT_OFF & _LVP_ON & _HS_OSC & _CP_ALL & _CPD_ON & _DEBUG_OFF
#endif //_ICD2

#ifdef _SIM
#pragma message "_SIM - defined, assuming use of simulator and plugins from SourceBoost IDE"
#endif // _SIM

#pragma DATA _IDLOC , 0x00, 0x01, 0x00, 0x00   //0100, only lower nibbles of each byte are used
#pragma CLOCK_FREQ	20000000

/**************************************************************************************
 *  DEVICE: 16F877A,  20mhz (no internal osc avail)
 *  		for use with PIC-MT-USB from olimex.com 
 *			info  - [http://www.olimex.com/dev/pic-mt-usb.html]
 *			schem - [http://www.olimex.com/dev/images/pic-mt-usb-sch.gif]
 *
 *  REWORK: Pull 'test' pin on FT232BM/U3 low to avoid test mode and allow default
 *	        enumeration to happen. On more recent boards I've ordered, it seems that
 *		    they're coming w/the fix. You may not have to perform the rework.
 *          
 *  PINOUT: 
 *							        +----U----+
 *					       Vpp/MCLR | 1    40 |  RB7 - ICSP PGD
 *				             - RA0	| 2    39 |  RB6 - ICSP PGC
 *	                         - RA1	| 3    38 |  RB5 - Button B1, Top
 *				             - RA2	| 4    37 |  RB4 - Button B2, Bot
 *				             - RA3	| 5    36 |  RB3 - ICSP PGM
 *	                         - RA4  | 6    35 |  RB2 - LED1, Green LED
 *		                     - RA5	| 7    34 |  RB1 - LED2, Red LED
 *						     - RE0	| 8    33 |  RB0 - 
 *					         - RE1	| 9    32 |  +V
 *					    	 - RE1	| 10   31 |  GND
 *                			    +V  | 11   30 |  RD7 - LCD D7
 *                			   GND  | 12   29 |  RD6 - LCD D6
 *                             OSC  | 13   28 |  RD5 - LCD D5
 *	                           OSC  | 14   27 |  RD4 - LCD D4
 *							 - RC0  | 15   26 |  RC7 - Serial RX
 *							 - RC1  | 16   25 |  RC6 - Serial TX
 *							 - RC2  | 17   24 |  RC5 - 
 *					I2C  SCL - RC3  | 18   23 |  RC4 - I2C SDA
 *					LCD   RS - RD0  | 19   22 |  RD3 - LCD Backlight
 *				    LCD R/~W - RD1  | 20   21 |  RD2 - LCD E
 *			        				+---------+
 *					 
 * 
 * function by pin:
 * 
 *   RC4 - SDA for i2c
 *   RC3 - SCL for i2c
 *
 *   RB1 - Red LED, active high/source
 *   RB2 - Green LED, active high/source
 *   RB5 - Top Button (B1) aka 'Prev', active low
 *   RB4 - Bottom Button (B2) aka 'Next', active low
 *
 *   RD0 - LCD RS (Register Select)
 *   RD1 - LCD R/~W
 *   RD2 - LCD E
 *   RD3 - LCD Backlight, active high
 *   RD4 - LCD D4
 *   RD5 - LCD D5
 *   RD6 - LCD D6
 *   RD7 - LCD D7
 *   
 **************************************************************************************/
  

//----------------------------------  Globals -----------------------------------------
sysTime_t sysCnt;

const 	 uint16_t maxActivityLEDPersist  = 150;

volatile bit	  flagPrevErrCode = false;			// indicates that prev err in list should be displayed
volatile bit 	  flagNextErrCode = false;				// inidicates that next err in list should be displayed
volatile bit	  flagClearCapturedData = false;		// indicates that both buttons were pressed simultaneously and passed debounce
volatile bit 	  flag1ms = false;					// set true every 1ms
volatile bit	  flagValidI2CData = false;

//--------------------------------  I/O pin definitions ------------------------------

#ifdef _SIM
const    bit buttonPressed = 1; 		// the board has N.O. switches with pu's
#pragma message "_SIM - buttonPressed = 1, simulator plugin has opposite active logic of dev board"
#else
const	 bit buttonPressed = 0;
#endif // _SIM

volatile bit buttonPrev@PORTB.5;
volatile bit buttonNext@PORTB.4;
volatile bit trisButtonPrev@PORTB.5;
volatile bit trisButtonNext@PORTB.4;

const uint8_t maxButtonDebounce = 40;			// countdown
uint8_t cntButtonPrevDebounce = maxButtonDebounce;			// defined in milliseconds
uint8_t cntButtonNextDebounce = maxButtonDebounce;
uint8_t cntButtonBothDebounce = maxButtonDebounce;

const uint16_t maxLockout = 300;	// countdown
uint16_t cntButtonLockout = 0;			// defined in milliseconds

volatile bit gie@INTCON.GIE;		// global int enable bit
inline void ei(void) { gie = 1; }	// enable ints
inline void di(void) { gie = 0; }	// disable ints

// T2 bits
volatile bit tmr2ie@PIE1.TMR2IE;
volatile bit peie@INTCON.PEIE;
volatile bit tmr2if@PIR1.TMR2IF;

// i2c bits
volatile uint8_t i2cBuff;
//const uint8_t	i2cDeviceAddress = 0xA0;  // old value
const uint8_t	i2cDeviceAddress = 0x60;

volatile bit 	trisScl@TRISC.3;
volatile bit 	trisSda@TRISC.4;
volatile bit 	sspie@PIE1.3;
volatile bit 	sspif@PIR1.3;
volatile bit 	sspen@SSPCON.5;

volatile bit 	sspov@SSPCON.6;		// buffer overflow
volatile bit 	bf@SSPSTAT.0;		// buffer full
volatile bit 	da@SSPSTAT.5;		// byte is sspbuf is 0=data, 1=address
volatile bit    rw@SSPSTAT.2;		// read=1, write=0

cInfo_t codeInfo;					// hold captured codes
cInfo_t codeBuff[_MAX_BUFFSIZE];
uint8_t next;
uint8_t idx;

ledInfo_t li;

void main()
{
oldCbInit();	// cleanup for v2.0
ledInit();
timeBaseInit();						// T2 is the clock source
i2cInit();
lcdInit();

lcdBacklightOn(3000);
lcdWipeLine1();
lcdPuts("ddcErrCodeReader");
lcdWipeLine2();
lcdPuts("v1.01");

// TODO: is there a bug which strcpy(rom char *) doesn't work right ?  simulator works okay, but it doesn't work on hardware

trisButtonPrev = Input;		// TODO: move this to buttonInit() once we implement the 1MS thingy
trisButtonNext = Input;

ei();

while(1) {

	if(flag1ms) {
				
		led1MSUpdate(); // update leds
		
		// button1MSUpdate(); // TODO: implement
		lcd1MSUpdate(); // TODO: implement

		// check for button presses
		if(!cntButtonLockout) {										// if we're not 'locked out' go ahead and check for keypresses
			if( (buttonPrev == buttonPressed) && (buttonNext == buttonPressed) ) {
				cntButtonPrevDebounce = cntButtonNextDebounce = maxButtonDebounce; 	// both pressed ? reset single button counters
				if(cntButtonBothDebounce) {
					flagClearCapturedData = false;
					cntButtonBothDebounce--;
					}
				else {
					flagClearCapturedData = true;
					cntButtonBothDebounce = maxButtonDebounce;
					cntButtonLockout = maxLockout;
					}
				}
			else { 																	// single button is currently being pressed
				cntButtonBothDebounce = maxButtonDebounce;							// reset the 'both button' counter
				if( buttonPrev == buttonPressed ) {
					if(cntButtonPrevDebounce) {
						flagPrevErrCode = false;
						cntButtonPrevDebounce--;
						}
					else {
						flagPrevErrCode = true;
						lcdBacklightOn(5000);
						cntButtonPrevDebounce = maxButtonDebounce;
						cntButtonLockout = maxLockout;			
						}
					}
				if( buttonNext == buttonPressed ) {
					if(cntButtonNextDebounce) {
						flagNextErrCode = false;
						cntButtonNextDebounce--;
						}
					else {
						flagNextErrCode = true;
						lcdBacklightOn(5000);
						cntButtonNextDebounce = maxButtonDebounce;
						cntButtonLockout = maxLockout;
						}
					}			
				}
			} 
		else {
			cntButtonLockout--;
			}
		flag1ms = false;
		}
		
	/* idle ---------------------------------------------------  */ 
	
	if(flagValidI2CData) {
		flagValidI2CData = false;
		
		li.color = RED;
		li.action = ON;
		li.onDuration = 0;
		ledSetState(&li);		// just got a valid error code!
		li.color = GREEN;
		li.action = ON;
		li.onDuration = maxActivityLEDPersist;
		ledSetState(&li);		// flicker the activity light - this could go into isr but I
								// want to avoid calling functions from the isr
		lcdBacklightOn(50);		// chirp backlight also
		
		codeInfo.code = i2cBuff;			// add a 'timestamp' to data once we've captured it
		codeInfo.ms   = sysCnt.ms;
		codeInfo.sec  = sysCnt.sec;
		
		oldCbPut(&codeInfo);

		}
	
	if(flagClearCapturedData) {
		flagClearCapturedData = false;
		oldCbInit();							// wipe out err code stack
		lcdClear();
		
		li.color = RED;
		li.action = OFF;
		ledSetState(&li);
		}
		
	if(flagNextErrCode) {
		flagNextErrCode = false;
		if(!cbReadNext(&codeInfo)) {
			// display code
			lcdWipeLine1();
			lcdPuts("0x");
			lcdWriteHex(codeInfo.code);
			lcdWriteChar(' ');
			lcdWriteDec(codeInfo.sec);
			lcdWriteChar('.');
			lcdWriteDec(codeInfo.ms);
			lcdWriteChar('s');

			// display text lookup
			uint8_t buff[32]; // TODO: reduce size later
			errFetch(buff, codeInfo.code);
			lcdWipeLine2();
			lcdPuts(buff);			
			}
		else {
			lcdWipeLine2();
			lcdWipeLine1();
			lcdPuts("no codes");
			}
		}
		
	if(flagPrevErrCode) {	     // TODO : for v2.0 this will be and 'extended descriptor' button where it's possible 
		flagPrevErrCode = false; //        to cycle through a long description of the code received.
		//cbTest();
		}		
	}
}

void interrupt(void) {

	if(tmr2if) {
		flag1ms = true;

		sysCnt.ms++;
		
		if(sysCnt.ms >= 1000) {
			sysCnt.ms = 0;
			if(sysCnt.sec != 0xFFFF)
				sysCnt.sec++;  		// 'cap' sec/ms in case of overflow
			}
	
		tmr2if = false;	
		}
		
	if(sspif) {
		
		if(bf) {
			i2cBuff = sspbuf;  			// clear bf
			da ? flagValidI2CData = true : flagValidI2CData = false;  // check for addresss bit, if da=1 then rcvd byte was data
			}

		if(sspov) sspov = false;		// if set, must be cleared in s/w so we generate acks
		sspif = false;
		}
}

void i2cInit(void) {

	trisScl = Input;
	trisSda = Input;

	sspadd  = i2cDeviceAddress;	// TODO: v2.0 fetch this from a location in eeprom for quick field upgrades
	sspcon  = ( (0<<WCOL) | (0<<SSPOV) | (0<<SSPEN) | (1<<CKP) | (00000110b) );
	
	i2cBuff = 0x00;
	
	sspif   = false;
	sspen   = true;		// turn on i2c functionality at RC3/4
	peie	= true;
	sspie   = true; 	// allow ints

}

void timeBaseInit(void) {		// setup 1ms tick period
	t2con  = 0x0;				// start w/clean slate
	tmr2   = 0x0;
	tmr2ie = true;
	peie   = true;
	tmr2if = false;
	pr2    = 156;				// XXX.Xus roll rate based on values set below 
	t2con  = 00111101b;		    // prescale by 8 - postscale by 4 - start timer
}

void showLcdBanner(void) {	

}


// TODO: pull this stuff out and add capability of module to track it's own index for use w/prev,next
// TODO: since enums now are able to typedef, return an enum as possible error codes.

void oldCbInit(void) { 
uint8_t j=0;
cInfo_t * ci;

	while(j < _MAX_BUFFSIZE) {
		ci = &codeBuff[j];
		ci->code = 0x00;
		ci->sec  = 0x0000;
		ci->ms   = 0x0000;
		j++;
		}

	next = 0x00;		// index to next open cInfo_t
	idx  = 0x00;		// index to 
}

uint8_t oldCbPut(cInfo_t * d) { // no get() since this is a put only buffer
cInfo_t * ci;

	if(next >= _MAX_BUFFSIZE) { return 0xff; } // full - ignore add
	
	ci = &codeBuff[next];
	ci->code = d->code;
	
	if(next == 0) {  // first entry resets time counters
		ci->sec    = 0;
		ci->ms     = 0;
		sysCnt.ms  = 0;
		sysCnt.sec = 0;
	} else {
		ci->sec  = d->sec;
		ci->ms   = d->ms;
	}
		
	next++;
	return 0x00; 
}

uint8_t cbReadNext(cInfo_t * d) { // read buff @ current idx index and inc
cInfo_t * ci; 

	if(next == 0) { return 0xff; } // empty
	
	ci = &codeBuff[idx];
	d->code = ci->code;
	d->sec  = ci->sec;
	d->ms   = ci->ms;
	
	idx++;
	if(idx >= _MAX_BUFFSIZE) { idx = 0; }  // wrap checks
	if(idx >= next)          { idx = 0; }
		
	return 0x00;
}

uint8_t cbReadPrev(cInfo_t * d) { // dec and read buff @ current idx index
	// not implemented yet
	return 0xff;
}

