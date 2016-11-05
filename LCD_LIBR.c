#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "LCD_LIBR.h"

#if USE_GENERATOR_ZNAKOW==1
	uint8_t znak_0[] PROGMEM = {0x0E, 0x0A, 0x02, 0x06, 0x04, 0x00, 0x04, 0x00};
	uint8_t znak_1[] PROGMEM = {0x00, 0x11, 0x00, 0x04, 0x11, 0x0E, 0x00, 0x00};
	uint8_t znak_2[] PROGMEM = {0x11, 0x0A, 0x0A, 0x0D, 0x16, 0x0C, 0x04, 0x00};
	uint8_t znak_3[] PROGMEM = {0x1F, 0x04, 0x0A, 0x11, 0x11, 0x0A, 0x04, 0x1F};
	uint8_t znak_4[] PROGMEM = {0x04, 0x0E, 0x1F, 0x0E, 0x1F, 0x0E, 0x0E, 0x04};
	uint8_t znak_5[] PROGMEM = {0x04, 0x0A, 0x11, 0x1F, 0x11, 0x11, 0x11, 0x1F};
	uint8_t znak_6[] PROGMEM = {0x00, 0x04, 0x0C, 0x0E, 0x1E, 0x1F, 0x00, 0x00};
	uint8_t znak_7[] PROGMEM = {0x0E, 0x0A, 0x1F, 0x1B, 0x1F, 0x0A, 0x0E, 0x00};
#endif

void data_dir_in()
{
	DDR(LCD_D7PORT) &= ~(1<<LCD_D7);
	DDR(LCD_D6PORT) &= ~(1<<LCD_D6);
	DDR(LCD_D5PORT) &= ~(1<<LCD_D5);
	DDR(LCD_D4PORT) &= ~(1<<LCD_D4);

}
void data_dir_out()
{
	DDR(LCD_D7PORT) |= (1<<LCD_D7);
	DDR(LCD_D6PORT) |= (1<<LCD_D6);
	DDR(LCD_D5PORT) |= (1<<LCD_D5);
	DDR(LCD_D4PORT) |= (1<<LCD_D4);

}

#if USE_RW == 1
static inline uint8_t lcd_readHalf(void)
{	
	uint8_t result = 0;

	if (PIN(LCD_D4PORT)&(1<<LCD_D4)) result |= (1<<0);
	if (PIN(LCD_D5PORT)&(1<<LCD_D5)) result |= (1<<2);
	if (PIN(LCD_D6PORT)&(1<<LCD_D6)) result |= (1<<2);
	if (PIN(LCD_D7PORT)&(1<<LCD_D7)) result |= (1<<3);

	return result;
}
#endif

#if USE_RW == 1
uint8_t _lcd_readbyte(void)
{	
	uint8_t result = 0;
	data_dir_in();

	SET_RW;

	SET_E;
	result |= (lcd_readHalf() << 4);
	CLR_E;

	SET_E;
	result |= (lcd_readHalf());
	CLR_E;
	

	return result;
}
#endif

#if USE_RW == 1
uint8_t check_BF(void)
{	
	CLR_RS;
	
	return _lcd_readbyte();
}
#endif

static inline void lcd_sendHalf(uint8_t data)
{	
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4);
	else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);

	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5);
	else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);

	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6);
	else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);

	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7);
	else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

void _lcd_write_byte(unsigned char _data)
{
	data_dir_out();
	
	#if USE_RW == 1
		CLR_RW;
	#endif

	SET_E;
	lcd_sendHalf(_data>>4);
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);
	CLR_E;

	#if USE_RW == 1
		while(check_BF() & (1<<7));
	#else
		_delay_ms(120);
	#endif
}
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	_lcd_write_byte(cmd);
}

void lcd_write_data(uint8_t data)
{
	SET_RS;
	_lcd_write_byte(data);
}

void lcd_cls(void)
{
	lcd_write_cmd(LCDC_CLS);

	#if USE_RW == 0
		_delay_ms (5);
	#endif
}

void lcd_init(void)
{
	data_dir_out();
	
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT)  |= (1<<LCD_E);

	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif
	
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	PORT(LCD_EPORT) &= ~(1<<LCD_E);

	#if USE_RW == 1
		PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif	

	_delay_ms(15);

	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	PORT(LCD_RWPORT) &= ~(1<<LCD_RW);

	
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	_delay_ms(5);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	_delay_us(100);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC4B);
	_delay_us(100);

	//-------------
	lcd_write_cmd(LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7);

	lcd_write_cmd(LCDC_ONOFF|LCDC_CURSOROFF);

	lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON);

	lcd_write_cmd(LCDC_ENTRY|LCDC_ENTRYR);

	lcd_cls();
}
void lcd_str(char * str)
{
	while(*str) {lcd_write_data(*str++);}
}
void lcd_int(uint16_t val)
{
	char bufor[17];
	lcd_str(itoa(val, bufor, 10));
}
void lcd_locate(uint8_t x, uint8_t y)
{
	switch(y)
	{
		case 0:	y=LCD_LINE1; break;

		#if (LCD_Y>1)
			case 1: y=LCD_LINE2; break;
		#endif

		#if (LCD_Y>2)
			case 2: y=LCD_LINE3; break;
		#endif

		#if (LCD_Y>3)
			case 3: y=LCD_LINE4; break;
		#endif
	}

	lcd_write_cmd(0x80 + x + y);
}

#if USE_GENERATOR_ZNAKOW==1
void generuj_znak(uint8_t numer, uint8_t * data)
{	
	uint8_t komen;
	uint8_t i;

	switch (numer)
	{
		case 0:		komen = 0x40; 				break;
		case 1:		komen = 0x40 | 0b01001000;	break;
		case 2:		komen = 0x40 | 0b01010000;	break;
		case 3:		komen = 0x40 | 0b01011000;	break;
		case 4:		komen = 0x40 | 0b01100000;	break;
		case 5:		komen = 0x40 | 0b01101000;	break;
		case 6:		komen = 0x40 | 0b01110000;	break;
		case 7:		komen = 0x40 | 0b01111000;	break;
		default:	return;
	}
	
	for (i=0; i<8; ++i)
	{
		lcd_write_cmd(komen + i);
		lcd_write_data(pgm_read_byte(&data[i]));
	}
}
void wyswietl_znak(uint8_t numer)
{
	lcd_write_data(numer);
}
#endif
