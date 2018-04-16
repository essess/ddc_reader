#ifndef _ERRTEXT_H_
#define _ERRTEXT_H_

typedef struct {
	uint8_t shortInfo[16];
	uint8_t longInfoTop[16];
	uint8_t longInfoBot[16];
	} il_t;  // il == infoLookup

void errFetch(uint8_t * const, const uint8_t);

void errLookup(il_t * const, const uint8_t);			// pushed to v2.0

#endif // _ERRTEXT_H_