/*
 * Game of Life in AVR C for PCD8544 LCD controller
 *
 * Vasco Flores vasco.flores@gmail.com
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define SCK_PIN  0b00100000
#define MISO_PIN 0b00010000
#define MOSI_PIN 0b00001000
#define SS_PIN   0b00000100

#define CE_PIN 0b10000000
#define RE_PIN 0b01000000
#define DC_PIN 0b00100000

#define CMD  0
#define DATA DC_PIN

#define LCD_WIDTH   84
#define LCD_HEIGHT  48
#define LCD_HEIGHT8 (LCD_HEIGHT >> 3)

// double buffer holding the cells
unsigned char buffer1[LCD_WIDTH * LCD_HEIGHT8];
unsigned char buffer2[LCD_WIDTH * LCD_HEIGHT8];

void send(unsigned char type, unsigned char data);
void clean();
void bitmapFull(const char * bmp);
void bitmapSprite(const unsigned char * bmp, unsigned char w, unsigned char h, unsigned char x, unsigned char y);

void send(unsigned char type, unsigned char data)
{
  // send command
	PORTD = CE_PIN | RE_PIN | type;

	PORTD &= ~CE_PIN;
  SPDR = data;
  while(!(SPSR & (1<<SPIF)));
	PORTD |= CE_PIN;
}

void clean()
{
  unsigned short i;

  send(CMD, 0x80);  // 0
  send(CMD, 0x40);  // 0

  for (i = 0; i < LCD_WIDTH * (LCD_HEIGHT/8); i++)
      send(DATA, 0x00);
}

void runGOF()
{
  unsigned char * old = buffer1, * new = buffer2, * p;
  unsigned short i;
  unsigned char j, xd, yd;
  unsigned char acc, nN, nS, nW, nE, nNW, nNE, nSW, nSE, x, xn;

  // randomize
  p = new;
  for (i = 0; i < LCD_WIDTH * LCD_HEIGHT8; i++)
      *(p++) = (unsigned char)rand();

	while (1)
  {
    // draw
    send(CMD, 0x80);
    send(CMD, 0x40);
    p = new;
    for (i = 0; i < LCD_WIDTH * LCD_HEIGHT8; i++)
        send(DATA, *(p++));

    _delay_ms(100);
    // swap buffer
    p = new;
    new = old;
    old = p;


    // iterate
    for (yd = 0; yd < LCD_HEIGHT8; yd++)
    {
      for (xd = 0; xd < LCD_WIDTH; xd++)
      {
        // current cell
        x = old[(yd * LCD_WIDTH) + xd];

        // neighbours XXX this ugly and buggy
        nN = (yd > 0) ? old[(yd - 1) * LCD_WIDTH + xd] : 0x00;
        nN = (x << 1) | ((nN & 0x80) >> 7);

        nS = (yd < (LCD_HEIGHT8 - 1)) ? old[((yd + 1) * LCD_WIDTH) + xd] : 0x00;
        nS = ((nS & 0x1) << 7) | (x >> 1);

        nW = (xd > 0) ? old[yd * LCD_WIDTH + (xd - 1)] : 0x00;

        nNW = (yd > 0) ? old[(yd - 1) * LCD_WIDTH + (xd - 1)] : 0x00;
        nNW = (nW << 1) | ((nNW & 0x80) >> 7);

        nSW = (yd < (LCD_HEIGHT8 - 1)) ? old[((yd + 1) * LCD_WIDTH) + (xd - 1)] : 0x00;
        nSW = ((nSW & 0x1) << 7) | (nW >> 1);

        nE = (xd < (LCD_WIDTH - 1)) ? old[(yd * LCD_WIDTH) + (xd + 1)] : 0x00;

        nNE = (yd > 0) ? old[(yd - 1) * LCD_WIDTH + (xd + 1)] : 0x00;
        nNE = (nE << 1) | ((nNE & 0x80) >> 7);

        nSE = (yd < (LCD_HEIGHT8 - 1)) ? old[((yd + 1) * LCD_WIDTH) + (xd + 1)] : 0x00;
        nSE = ((nSE & 0x1) << 7) | (nE >> 1);

        // calculate each bit of next gen
        xn = 0x0;
        for (j = 8; j; j--)
        {
          // add up neighbours
          acc = (nN & 0x1) + (nS & 0x1) + (nW & 0x1) + (nE & 0x1);
          acc+= (nNW & 0x1) + (nNE & 0x1) + (nSW & 0x1) + (nSE & 0x1);

          // check if cell should be alive
          xn >>= 1;
          if (acc == 3 || ((x & 0x1) && acc == 2))
              xn |= 0x80;
          
          nN >>= 1; nS >>= 1; nW >>= 1; nE >>= 1;
          nNW >>= 1; nNE >>= 1; nSW >>= 1; nSE >>= 1;
          x >>= 1;  
        }

        new[(yd * LCD_WIDTH) + xd] = xn;
      }
    }
  }
}

void randomize()
{
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
  ADMUX=0;
  ADCSRA |= (1<<ADSC);
  while (ADCSRA & (1<<ADSC));

  srand(ADC);

  ADCSRA = 0;
}

int main(void)
{
  // enable SPI
  DDRB = SS_PIN;
  PORTB = SS_PIN;
  SPCR = (1<<SPE)|(1<<MSTR);

  DDRB = SCK_PIN | MOSI_PIN | SS_PIN;
  PORTB = SCK_PIN | MOSI_PIN | SS_PIN;

  // reset LCD
  DDRD = CE_PIN | RE_PIN | DC_PIN;
	PORTD = RE_PIN;
	PORTD = CE_PIN | RE_PIN;
	PORTD = CE_PIN;
  _delay_ms(100);
	PORTD = CE_PIN | RE_PIN;



  // configure LCD
  send(CMD, 0x21); //Tell LCD extended commands follow
  send(CMD, 0xB0); //Set LCD Vop (Contrast)
  send(CMD, 0x04); //Set Temp coefficent
  send(CMD, 0x13); //LCD bias mode 1:48 (try 0x13)

  send(CMD, 0x20); 
  send(CMD, 0x0C); //Set display control, normal mode.

  clean();
  randomize();
  runGOF();
  
  return 0;
}
