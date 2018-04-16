#include <system.h>
#include "types.h"
#include "lcd.h"

#ifdef _ICD2
#include <icd2.h>
#else
#endif // _ICD2

#pragma message "public functions available from this module are prefixed w/'lcd'"

volatile bit trisPinLcdE@TRISD.2;
volatile bit pinLcdE@PORTD.2;

volatile bit trisPinLcdRW@TRISD.1;
volatile bit pinLcdRW@PORTD.1;

volatile bit trisPinLcdRS@TRISD.0;
volatile bit pinLcdRS@PORTD.0;

volatile bit trisPinLcdD4@TRISD.4;
volatile bit pinLcdD4@PORTD.4;

volatile bit trisPinLcdD5@TRISD.5;
volatile bit pinLcdD5@PORTD.5;

volatile bit trisPinLcdD6@TRISD.6;
volatile bit pinLcdD6@PORTD.6;

volatile bit trisPinLcdD7@TRISD.7;
volatile bit pinLcdD7@PORTD.7;

const	 bit backlightOn = 1;
const	 bit backlightOff = !backlightOn;
volatile bit lcdBacklight@PORTD.3;
volatile bit trisLcdBacklight@TRISD.3;

volatile bit flagBacklightOn = false;
volatile bit flagIsLcdInitialized = false;
volatile bit flagLCDBusy = false;  // used when 'streaming' is implemented

volatile uint8_t * const tempLcdBuff = "[[unintialized]]";
volatile uint16_t backlightTimer = 0;

// ------------------  inlines  --------------------------

inline void setCmdMode(void)    { trisPinLcdRS = Output; pinLcdRS = 0; }
inline void setDataMode(void)   { trisPinLcdRS = Output; pinLcdRS = 1; }
inline void setReadState(void)  { trisPinLcdRW = Output; pinLcdRW = 1; }
inline void setWriteState(void) { trisPinLcdRW = Output; pinLcdRW = 0; }
inline void clockE(void)        { trisPinLcdE = Output; pinLcdE = 1; delay_us(10); pinLcdE = 0; }

// ------------------  end inlines  ----------------------

inline void lcdBacklightOn(void) {

	if(!flagBacklightOn) {
		flagBacklightOn = true;
		trisLcdBacklight = Output;
		lcdBacklight = backlightOn;
		}
}

void lcdBacklightOn(const uint16_t duration) {
	if(duration < 10) return;   // just return for stupid values
	
	if(!flagBacklightOn) {
		backlightTimer = duration;
		lcdBacklightOn();
	} else {
		// TODO:  what if the backlight is already on from a previous request ?
		//        for now, just ignore the request.
	}
	
}

inline void lcdBacklightOff(void) {

	if(flagBacklightOn) {
		flagBacklightOn = false;
		trisLcdBacklight = Output;
		lcdBacklight = backlightOff;
		}
}


void lcd1MSUpdate(void) {
	// TODO : when 'streaming' is implemented then this will check if lcd is busy, if not, pull next char from
	//        stream and write to lcd - also updates backlight timing
	backlightTimer ? backlightTimer-- : lcdBacklightOff();
	
}

void writeNibble(const uint8_t data) {  // bits 0-3
	
		setWriteState();
		
		trisPinLcdD7 = Output;
		trisPinLcdD6 = Output;
		trisPinLcdD5 = Output;
		trisPinLcdD4 = Output;
		
		testbit(data, 7) ? pinLcdD7 = 1 : pinLcdD7 = 0;
		testbit(data, 6) ? pinLcdD6 = 1 : pinLcdD6 = 0;
		testbit(data, 5) ? pinLcdD5 = 1 : pinLcdD5 = 0;
		testbit(data, 4) ? pinLcdD4 = 1 : pinLcdD4 = 0;
		
		clockE();
		delay_us(150);
		
}

void writeByte(const uint8_t data) {    // bits 0-8

		setWriteState();
		
		trisPinLcdD7 = Output;
		trisPinLcdD6 = Output;
		trisPinLcdD5 = Output;
		trisPinLcdD4 = Output;
		
		testbit(data, 7) ? pinLcdD7 = 1 : pinLcdD7 = 0;
		testbit(data, 6) ? pinLcdD6 = 1 : pinLcdD6 = 0;
		testbit(data, 5) ? pinLcdD5 = 1 : pinLcdD5 = 0;
		testbit(data, 4) ? pinLcdD4 = 1 : pinLcdD4 = 0;		
		
		clockE();
		
		testbit(data, 3) ? pinLcdD7 = 1 : pinLcdD7 = 0;
		testbit(data, 2) ? pinLcdD6 = 1 : pinLcdD6 = 0;
		testbit(data, 1) ? pinLcdD5 = 1 : pinLcdD5 = 0;
		testbit(data, 0) ? pinLcdD4 = 1 : pinLcdD4 = 0;
		
		clockE();
		
		delay_us(150);
		
}

void lcdInit(void) {		// TODO: look into #define'ing command set so this is a little clearer

	delay_ms(15);			// begin reset
	setCmdMode();
	writeNibble(0x03);
	delay_ms(5);
	writeNibble(0x03);
	writeNibble(0x03);		// reset complete
	writeNibble(0x02); 		// set interface length to 4bit
							// at completion, all writes are now byte writes
						
	writeByte(0x28);		// 4-bit, 2-line, 5x7
	writeByte(0x0C);		// display on, cursor off
	writeByte(0x06);		// inc, no shift
	writeByte(0x01);		// clear display
	delay_ms(4);	

	backlightTimer = 0;
	flagIsLcdInitialized = true;
}

void lcdWriteChar(const uint8_t data) {

	setDataMode();
	writeByte(data);
	
}

void lcdWriteHex(const uint8_t data) {
uint8_t temp;

	//lcdPuts("0x");
	temp = data;
	temp >>= 4;
	temp &= 0x0f;
	temp += '0'; //temp = ((data >> 4) & 0x0f) + '0';
	if(temp > '9') { temp += 7; } 
	lcdWriteChar(temp);
	temp = data;
	temp &= 0x0f;
	temp += '0'; //temp = (data & 0x0f) + '0';
	if(temp > '9') { temp += 7; }
	lcdWriteChar(temp);
	
}

void lcdWriteHex(const uint16_t data) {

	lcdWriteHex((uint8_t)((data >> 8) & 0x00ff));
	lcdWriteHex((uint8_t)(data & 0x00ff));
	
}

void lcdWriteHex(const uint32_t data) {	// TODO: untested, verify operation is correct
										// might have to think about breaking this up into uint8_t calls to save a level of stack space
	lcdWriteHex((uint16_t)((data >> 16) & 0x0000ffff));
	lcdWriteHex((uint16_t)(data & 0x0000ffff));
	
}

void lcdClear(void) {

	setCmdMode();
	writeByte(0x01);
	delay_ms(4);
	delay_us(200);
	
}

void lcdMoveToLine2(void) {

	setCmdMode();
	writeByte(0xc0);				// move to display address of 0x40
	
}

void lcdMoveToLine1(void) {

	setCmdMode();
	writeByte(0x80);				// move to display address of 0x00
	
}

void lcdWipeLine1(void) {
	
	lcdMoveToLine1();
	lcdPuts("                ");
	lcdMoveToLine1();
}

void lcdWipeLine2(void) {

	lcdMoveToLine2();
	lcdPuts("                ");
	lcdMoveToLine2();
}


// TODO: override this to accept rom data type and do auto copy into 16byte buffer before dumping out to lcd

void lcdPuts(rom char * const data) {

	strcpy(tempLcdBuff, data);
	lcdPuts(tempLcdBuff);
}

void lcdPuts(const uint8_t * const data) {  // put string at current cursor position

uint8_t j;

	for(j=0;data[j];j++)
		lcdWriteChar(data[j]);

}

void lcdWriteDec(const uint32_t data)	// I'm sure there is a more elegant way to implement this, but I have no time
{										// search on binary->BCD conversion for examples
	bit flagSkipLeadingZeros = true;	// skip initial zeros in number
	uint32_t temp;
	uint32_t power;
	uint32_t tempData;
	
	if(!flagIsLcdInitialized) lcdInit();

	if(data)
	{
		tempData = data;				// don't break const restriction	
		power=1000000000;				// range of 0<=999,999,999
		
		do
		{
			power /= 10;				// shift
			temp = tempData / power;

			if ( (temp>0) || (flagSkipLeadingZeros==false) )
			{
				lcdWriteChar((uint8_t)(temp+'0'));
				flagSkipLeadingZeros=false;
			}
			tempData -= temp * power;
		}
		while (power!=1);
	}
	else {
		lcdWriteChar('0');				// if passed 0, just dump a 0
	}
}

void lcdWriteDec(const uint16_t data) {
	lcdWriteDec((uint32_t)data);
}

void lcdWriteDec(const uint8_t data) {
	lcdWriteDec((uint8_t)data);
}