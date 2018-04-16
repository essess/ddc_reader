#ifndef _LED_H_
#define _LED_H_

// TODO: when compiler support function pointers, add a callback for when a blink cycle completes (good for morse code implementations :)

typedef enum { 
	RED    	= 0x01, 
	GREEN  	= 0x02  // TODO : add orange option
	} ledColors_t;
	
typedef enum {
	BLINK 	= 0x04,
	ON		= 0x05,
	OFF		= 0x06
	} ledAction_t;

typedef struct {
	ledColors_t	color;
	ledAction_t	action;
	uint16_t    onDuration;
	uint16_t	offDuration;
	} ledInfo_t;


void ledInit(void);
void led1MSUpdate(void);  		// used to update internal persist counters and change LED state 
								// if you don't use 'blink' actions or persist durations, 
								// then don't worry about calling this.

						
void ledSetState(const ledInfo_t * const);
void ledGetState(ledInfo_t * const);


//void ledSetState(void);
//void ledGetState(void);

#endif //_LED_H_