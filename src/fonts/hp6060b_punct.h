#ifndef HP6060B_PUNCT_H_
#define HP6060B_PUNCT_H_
/*
* THIS IS COLUMN MAJOR FONT
* application : SAMSUNG KS0108, NOVATEK NT7502, EPSON SED1520 (Small LCD Panel 128x64,100x32 etc.)
*
* This font is for HP6060B Punctuations 
*
* written by ssk.
* File Name           : hp6060b_punct.h
* Date                : 9:12 PM 12/5/2019
*
*/  

/* 
* Fixed width font for numbers                                           
*                                                                       
* This font is very useful when using overstrike as all characters & numb
* are all the same width.                                                
*                                                                        
* This font also contains a few special characters that are nice for cert
* like clocks, signed values or decimal point values.                    
*                                                                         
*/
#include <avr/pgmspace.h>

const uint8_t hp6060b_punct[] PROGMEM =
{
	0x0,0x0  , // Data Size (high byte, low byte)
	2,         // Character width  ('0' is Variable width font, use below width table each character)
    7,         // Character height (pixel)
    32,        // First character  (' ')
    35,        // End character    ('▼')

    0x00, 0x00,// (space) /* ' ' */
	0xA0, 0x60,// ,       /* ',' */
    0xC0, 0xC0,// .       /* '.' */
	0x6C, 0x6C,// :       /* ':' */
};
extern const uint8_t hp6060b_punct[] PROGMEM;
#endif

