#include <system.h>
#include "types.h"
#include "cb.h"
#include "lcd.h" // TODO: remove after testing

#pragma message "public functions available from this module are prefixed w/'cb'"

const uint8_t  fillChar = '\0';

void cbInit(cb_t * const s) {
uint8_t j;

	for(j=0; j < _MAX_SIZE ; j++)  // wipe out buff  // TODO: swap out j for i and see what happens
		s->buff[j] = fillChar;

	s->item = 0x00;
	s->tail = 0x00;
	s->head = 0x00;

}

ec_t cbPut(cb_t * const s) {  

	if((s->head - s->tail) >= _MAX_SIZE)
		return(eBuffFull);
	
	uint8_t top = s->head;	// split into a few operations because the compiler chokes
	top &= (_MAX_SIZE-1);   // this is the reason we have a power of 2 restriction on buff size
	
	s->buff[top] = s->item;
	
	top++;
	s->head = top;
	
	return(eOK);	
}


ec_t cbGet(cb_t * const s) {

	if(s->head == s->tail)
		return(eBuffEmpty);

	uint8_t bot = s->tail;
	bot &= (_MAX_SIZE-1);		// mask it
	
	s->item = s->buff[bot];
	
	bot++;
	s->tail = bot;
	
	return(eOK);
}

void cbTest(void) {

cb_t s;
uint8_t j=0;

starttest:

cbInit(&s);

if(cbGet(&s) != eBuffEmpty)
	hang();

do {
	s.item = j++;
} while (cbPut(&s) != eBuffFull);

while(cbGet(&s) != eBuffEmpty) {
	lcdWipeLine2();
	lcdWriteHex(s.item);
	delay_ms(250);
	}

do {	// BREAKS :  investigate
	s.item = j++;
} while (cbPut(&s) != eBuffFull);

while(cbGet(&s) != eBuffEmpty) {
	lcdWipeLine2();
	lcdWriteHex(s.item);
	delay_ms(250);
	}
	
		
goto starttest;
// the only way to make it here is for correct operation, use debugger to
// verify ec here
//hang();

}