#ifndef _DDCERRCODEREADER_H_
#define _DDCERRCODEREADER_H_

#define _IDLOC			0x2000	// only the lower nibbles from add 0x2000->0x2003 are used
#define _MAX_BUFFSIZE	8

void timeBaseInit(void);		// init T2 for mod operation
void i2cInit(void);

typedef struct {	// updated in isr by ms tick counts
	uint16_t ms;	
	uint16_t sec;
	} sysTime_t;

typedef struct {	// code buffer
	uint8_t	 code;
	uint16_t sec;
	uint16_t ms;
	} cInfo_t;



void oldCbInit(void);
uint8_t oldCbPut(cInfo_t *);
uint8_t cbReadNext(cInfo_t *);
uint8_t cbReadPrev(cInfo_t *);

#endif //_DDCERRCODEREADER_H_
