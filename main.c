#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "LCD_LIBR.h"

	

int main()
{
	lcd_init();
	
	

	//char napis[11]="          \0";
	//uint8_t nr_litery = 0;


	
	lcd_str("Program Testowy");
	lcd_locate(0, 1);
	lcd_str("A");

	generuj_znak(0, znak_0);
	generuj_znak(1, znak_1);
	generuj_znak(2, znak_2);
	generuj_znak(3, znak_3);
	generuj_znak(4, znak_4);
	generuj_znak(5, znak_5);
	generuj_znak(6, znak_6);
	generuj_znak(7, znak_7);
	
	lcd_locate(5, 1);
	wyswietl_znak(0);
	wyswietl_znak(1);
	wyswietl_znak(2);
	wyswietl_znak(3);
	wyswietl_znak(4);
	wyswietl_znak(5);
	wyswietl_znak(6);
	wyswietl_znak(7);
	lcd_str("Q");
	while(1)
	{

		


	}

	return 0;
}
