/*
 * $Id: main.c 3:06 PM 11/28/2019 ssk  $
 *
 * HP 6060B Graphic LCD .
 *
 * MIT License
 *
 * Copyright (c) 2019 ssk.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <string.h>           // memset
#include "sbn166g.h"
#include "glcd.h"
#include "hp6060b.h"
#include "spi.h"
#include "fonts/allfonts.h"
#include "bitmaps/allbitmaps.h"

static void setup(void);
static void timer1_init(void);
static void welcome(void);

static void MF_InitFrameBuffer(void);
static void MF_DisplayDigit(void);
static void MF_DisplayPunctuation(void);
static void MF_DisplayAnnunciator(void);
static uint8_t MF_DigitLookup(uint8_t data);
static uint8_t MF_PunctuationLookup(uint8_t data);
static uint8_t MF_isValid(void);

tMessageFrame tMF[MF_SZ_COMMAND];

volatile uint16_t milliseconds=0;

#ifdef __DEBUG_MODE__
#include <stdio.h>            // FILE
#include "uart_simple.h"
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
uint16_t lastActiveTime;
#endif

int main(void)
{
  /*
   * Enable the watchdog with a period of 2s
   * Watchdog Timer is initially disabled,
   * but can be enabled by writing the WDE bit to 1 without any restriction.
   */
  wdt_enable(WDTO_2S);

  setup();
  MF_InitFrameBuffer();

  sei();
#ifdef __DEBUG_MODE__
  stdout = &mystdout;
  uart_init();
#endif

  wdt_reset();
  welcome();
  wdt_reset();
  
  while(1)
  {
    /*
     * when PWO logic 'L' (data bus idle) Refreshes the display
     */
    //if((milliseconds & 0x200) && isDataBusIdle())
    if(isDataBusIdle())
    {
      if(MF_isValid())
      {
#ifdef __DEBUG_MODE__
        lastActiveTime = milliseconds;
#endif
        MF_DisplayDigit();
        MF_DisplayPunctuation();
        MF_DisplayAnnunciator();
        MF_InitFrameBuffer();
#ifdef __DEBUG_MODE__
        printf("lead time(ms) = [%u]\r\n",milliseconds-lastActiveTime);
#endif
      }
      wdt_reset();
    }
  }
  return 0;
}

static void welcome(void)
{
  glcd_clear(0x00);
  glcd_selectfont(lcd14_15bi_16x17, LCD_DOT_SET, FONT_ENGLISH,0);
  glcd_gotoxy(0,8); glcd_puts_P("************");

  glcd_selectfont(system_5_5x7, LCD_DOT_SET, FONT_ENGLISH,12);
  glcd_gotoxy(4 ,24);
  for(uint8_t i=0; i<12; i++)
  {
    glcd_putc(MF_ANNUNCIATOR_CHAR);
  }
  
  /*
   * Display check Puase roughly 500mS (0.5s)
   * the Interrupts routine will run without pausing.
   */
  while(1)
  {
    if(milliseconds & 0x200)
    {
      break;
    }
  }
  wdt_reset();

  glcd_clear(0x00);
  glcd_bitmap(hp52x32, 0,0,LCD_DOT_SET);
  glcd_selectfont(system_5_5x7, LCD_DOT_SET, FONT_ENGLISH,1);
  glcd_gotoxy(53,8);  glcd_puts_P("6060B    3-60V/0-60A 300W");
  glcd_gotoxy(53,16); glcd_puts_P("SYSTEM DC ELECTRONIC LOAD");
  
  /*
   * At start up, a momentary (1 second) welcome screen display
   * the Interrupts routine will run without pausing.
   */
  while(1)
  {
    if(milliseconds & 0x400)
    {
      break;
    }
  }  
  wdt_reset();
  glcd_clear(0x00);
}

static void timer1_init(void)
{
  // Timer 0 interrupt gets called every 0.001sec(1ms)
  TCCR1B =  _BV(WGM12) |   // CTC mode(mode:4)
            _BV(CS10);     // clkI/O/1 (no prescaler)
            
  OCR1A  |=  15999;        //(0.001    / (1 / (16000000 / 1)) - 1)
  TIMSK  |= _BV(OCIE1A);
}

/*
* Global variable wrapper.
*
* you may need to change some global values at run time
* global Variable  must be managed one place
*
*/

static void setup(void)
{
/*
* configure external interrupts
*
* INT0(The falling edge)  , PWO :global chip select
* INT1(Any logical change), SYNC:1=read data on ISA(command)
*                                0=read data on INA(data)
*/
  MCUCR |= _BV(ISC01) |      // INT0, The falling edge
           _BV(ISC10);       // INT1, Any logical change

  // clear external interrupts flags INT1, INT0
  GIFR  |= _BV(INTF1) | _BV(INTF0);

  //  enable external interrupts INT1, INT0
  GICR  |= _BV(INT1)  | _BV(INT0);

  // GPIO initialize
  CTRL_DDR  &= ~(_BV(CTRL_PWO) | CTRL_SYNC);  // input
  CTRL_PORT |=  (_BV(CTRL_PWO) | CTRL_SYNC);  // enable internal pull-up register

  timer1_init();
  glcd_init();

  // SPI in slave mode (LSB fist)
  spi_init(SPI_MODE_0, SPI_LSB, SPI_INTERRUPT, SPI_SLAVE);
}

static void MF_InitFrameBuffer(void)
{
  memset(&tMF, 0, sizeof(tMF));
}

static uint8_t MF_isValid(void)
{

  if((tMF[MF_IDX_REGISTER_A].valid  == MF_DATA_VALID) &&
     (tMF[MF_IDX_REGISTER_B].valid  == MF_DATA_VALID) &&
     (tMF[MF_IDX_REGISTER_C].valid  == MF_DATA_VALID) &&
     (tMF[MF_IDX_ANNUNCIATOR].valid == MF_DATA_VALID))
  {
    return MF_DATA_VALID;
  }
  return MF_DATA_INVALID;

}

// number or character
static void MF_DisplayDigit(void)
{
  uint8_t data;

  glcd_selectfont(lcd14_15bi_16x17, LCD_DOT_SET, FONT_ENGLISH,0);
  glcd_gotoxy(0,8);
  for(uint8_t i=0; i<MF_MAX_DIGIT; i+=2)
  {
    // even digit
    data = ((tMF[MF_IDX_REGISTER_A].data[i/2] & 0xf0) >> 4)|
            (tMF[MF_IDX_REGISTER_B].data[i/2] & 0x30)      |
           ((tMF[MF_IDX_REGISTER_C].data[i/2] & 0x10) << 2);
    glcd_putc(MF_DigitLookup(data));

    // odd digit
    data =  (tMF[MF_IDX_REGISTER_A].data[i/2] & 0x0f)      |
           ((tMF[MF_IDX_REGISTER_B].data[i/2] & 0x03) << 4)|
           ((tMF[MF_IDX_REGISTER_C].data[i/2] & 0x01) << 6);
    glcd_putc(MF_DigitLookup(data));
  }
}

// Punctuation ('.', ',', ':')
static void MF_DisplayPunctuation(void)
{
  uint8_t data;

  glcd_selectfont(hp6060b_punct, LCD_DOT_SET, FONT_ENGLISH,15);
  glcd_gotoxy(14,16);
  for(uint8_t i=0; i<MF_MAX_DIGIT; i+=2)
  {
    // even digit punctuation
    data = (tMF[MF_IDX_REGISTER_B].data[i/2] & 0xc0);
    glcd_putc(MF_PunctuationLookup(data));

    // odd digit punctuation
    data = (tMF[MF_IDX_REGISTER_B].data[i/2] & 0x0c);
    glcd_putc(MF_PunctuationLookup(data));
  }
}

static void MF_DisplayAnnunciator(void)
{
  uint16_t bitmask;

  bitmask = (tMF[MF_IDX_ANNUNCIATOR].data[0] << 8) | (tMF[MF_IDX_ANNUNCIATOR].data[1]);

  glcd_selectfont(system_5_5x7, LCD_DOT_SET, FONT_ENGLISH,12);
  glcd_gotoxy(4 ,24);
  for(uint8_t i=MF_MAX_DIGIT; i--;)
  {
    if(bitmask & _BV(i))
    {
      glcd_putc(MF_ANNUNCIATOR_CHAR);
    }
    else
    {
      glcd_putc(' ');
    }
  }
}

/*
* Lookup table for non standard characters
*/
static uint8_t MF_DigitLookup(uint8_t data)
{
  if(data == 00) data = '*';   // '@' -> '*';
  else if(data < 0x20)  data = data + '@';   // '@ABCDEFGHIJKLMNOPQRSTUVWXYZ'
  else if(data == 0x2c) data = '<';    // should be a left pointing arrow
  else if(data == 0x2e) data = '>';    // should be a right pointing arrow
  else if(data == 0x3a) data = '*';    // should be the 14 segments lit
  else if(data <  0x40) data = data;   // ';<=>?'
  else if(data <  0x46) data = data - 'A' + 'a'; // 'abcde'
//else if(data == 0x40) data = '/';    // should be a 90° counter clockwise rotated 'T'
//else if(data == 0x46) data = '/';    // 46~4b this one and the 5 following should represent a hourglass
//else if(data == 0x47) data = '/';
//else if(data == 0x48) data = '/';
//else if(data == 0x49) data = '/';
//else if(data == 0x4a) data = '/';
//else if(data == 0x4b) data = '/';
//else if(data == 0x4c) data = '/';    // 'µ'
//else if(data == 0x4d) data = '/';    // "not equal"
//else if(data == 0x4e) data = '/';    // sigma sign
//else if(data == 0x4f) data = '/';
  else
  {
    data = '?'; //unknown
  }
  return data;
}
/*
* Lookup table for Punctuation characters
*/
static uint8_t MF_PunctuationLookup(uint8_t data)
{
  switch(data)
  {
    case 0x40:        // even digits
    case 0x04:        // odd digits
         data = MF_PUNCT_DOT;
         break;

    case 0x80:
    case 0x08:
         data = MF_PUNCT_COLON;
         break;

    case 0xc0:
    case 0x0c:
         data = MF_PUNCT_COMMA;
         break;

    default:
         data = MF_PUNCT_NONE;
         break;
  }
  return data;
}
/**********************
 * Interrupt routines *
 **********************/
ISR(TIMER1_COMPA_vect)
{
  ++milliseconds;
}
/*
* External interrupt 0(PWO Falling Edge)
*
* PWO:used to detect the end of the Chip Select of the display
* PWO Logic Low(1) - SPI Data bus Idle(SPI stop)
*/
ISR(INT0_vect)
{
  spi_disable();
}
/*
* External interrupt 1(SYNC Any logical change)
*
* SYNC:used to enable the SPI when the SYNC line from the 6060B changes state
* "1":command(ISA line), "0":data(INA line)
*/
ISR(INT1_vect)
{
  if(isDataBusActive())
  {
    spi_enable();
  }
}
/*
* SPI Serial Transfer Complete interrupt
*
* stores unprocessed data(Message Frame consists of 7 commands)
*
* idxCmd : current command index
* szData : data Size(one or more, max 6 byte)
* data   : command or data that corresponding to the command
*/
ISR(SPI_STC_vect)
{
  static uint8_t idxCmd = 0;
  static uint8_t szData = 0;
  uint8_t data;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    data = SPDR;

    if(isCommand())
    {
      /*
       * if SYNC Logic High(1), command field in the message frame(from ISA)
       */
      switch(data)
      {
        case MF_REGISTER_A:
             idxCmd = MF_IDX_REGISTER_A;
             szData = MF_SZ_REGISTER_A;
             break;

        case MF_REGISTER_B:
             idxCmd = MF_IDX_REGISTER_B;
             szData = MF_SZ_REGISTER_B;
             break;

        case MF_REGISTER_C:
             idxCmd = MF_IDX_REGISTER_C;
             szData = MF_SZ_REGISTER_C;
             break;

        case MF_ANNUNCIATOR:
             idxCmd = MF_IDX_ANNUNCIATOR;
             szData = MF_SZ_ANNUNCIATOR;
             break;

        // below an unused command - ignored
        case MF_START_MF:
        case MF_UNCHECK_2E0:
        case MF_DISPLAY_ONOFF:
        default:
             szData = 0;
             break;
      }
      if(szData > 0)
      {
        tMF[idxCmd].cmd = data;
        tMF[idxCmd].dsz = szData;
      }
    }
    else
    {
      /*
       * if SYNC Logic Low(0), one or more data that corresponding to the command(from INA)
       * it depends on the previous "command" received
       */
      if(szData > 0)
      {
        // data are received in reverse order, we have to reorder them
        tMF[idxCmd].data[szData-1] = data;
        tMF[idxCmd].valid = MF_DATA_VALID;  // data are received.
        szData--;
      }
      else
      {
        spi_disable();
      }
    }
  }
}

//We should never pass here ...
ISR(BADISR_vect)
{
  //printf("???(BADISR_vect)\n");
}
/*
 * EOF
 */