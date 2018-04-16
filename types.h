#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

typedef enum {
	eOK           = 0x00,
	eBuffFull     = 0x01, 
	eBuffEmpty    = 0x02,
	eBuffNotInitd = 0x03,
	eUnknown      = 0xFF
	} ec_t;

#define Input  1				// used for TRIS registers
#define Output 0


inline void hang(void) { 
asm hanghere:	goto hanghere
}


#define testbit(var, bit)   ((var) & (1 <<(bit)))		
#define setbit(var, bit)    ((var) |= (1 << (bit)))
#define clrbit(var, bit)    ((var) &= ~(1 << (bit)))

#define _paste(a,b)			a##b
#define bitof(var,num)		((var).bits._paste(b,num))

#endif // _TYPES_H_
