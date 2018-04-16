#ifndef _LCD_H_
#define _LCD_H_

void lcdWriteChar(const uint8_t);	// TODO: work on making/checking for const

void lcdWriteHex(const uint8_t);
void lcdWriteHex(const uint16_t);
void lcdWriteHex(const uint32_t);

void lcdWriteDec(const uint8_t);
void lcdWriteDec(const uint16_t);
void lcdWriteDec(const uint32_t);

void lcdInit(void);
void lcdClear(void);

void lcdMoveToLine1(void);
void lcdMoveToLine2(void);

void lcdPuts(const uint8_t * const);

void lcdWipeLine1(void);
void lcdWipeLine2(void);

void lcdBacklightOn(void);	
void lcdBacklightOn(const uint16_t);		
void lcdBacklightOff(void);

void lcd1MSUpdate(void);

#endif //_LCD_H_
