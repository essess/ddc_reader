#ifndef _CB_H_
#define _CB_H_

#define _MAX_SIZE	32

#if _MAX_SIZE < 2
#error "_MAX_SIZE must be 2>=128, (2,4,8,16,32,64,128)."
#elif TBUF_SIZE > 128
#error "_MAX_SIZE must be 2>=128, (2,4,8,16,32,64,128)."
#elif ((_MAX_SIZE & (_MAX_SIZE-1)) != 0)
#error "_MAX_SIZE must be a power of 2, (2,4,8,16,32,64,128)."
#endif

typedef struct {
	uint8_t      buff[_MAX_SIZE];
	uint8_t      item;					// used to pass back/in specific item into buff[]
	uint8_t      tail;					// bot of circ buff, pull index  -  cbGet()
	uint8_t      head;					// top of circ buff, the insert point of cbPut()
	} cb_t;

void cbInit(cb_t * const);		// inits user provided cb_t
ec_t cbPut(cb_t * const);		// return eBuffFull on error
ec_t cbGet(cb_t * const);		// return eBuffEmpty on error

void cbTest(void);

#endif //_CB_H_