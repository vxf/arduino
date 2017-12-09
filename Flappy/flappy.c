#include <stdlib.h>
#include <avr/io.h>
//#include <inttypes.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define SCK_PIN  0b00100000 // D13 - SCLK
#define MISO_PIN 0b00010000 // D12 - X
#define MOSI_PIN 0b00001000 // D11 - DN
#define SS_PIN   0b00000100 // D10 - X

#define CE_PIN 0b10000000   // D7 - SCE
#define RE_PIN 0b01000000   // D6 - RST
#define DC_PIN 0b00100000   // D5 - D\C
#define BUTTON_PIN 0b00010000

#define CMD  0
#define DATA DC_PIN

#define LCD_WIDTH   84
#define LCD_HEIGHT  48
#define LCD_HEIGHT8 (LCD_HEIGHT >> 3)

#define FLAPPY_WIDTH  17
#define FLAPPY_HEIGHT 16 >> 3

const unsigned char flappy1 [] = {
0xc0, 0x0, 0x30, 0x1, 0x28, 0x3, 0x24, 0x5, 0x22, 0x5, 0x22, 0x9, 0xc1, 0x8, 0x1, 0x8, 0x1d, 0x9, 0xa3, 0xa, 0x41, 0x5, 0x41, 0x5, 0x5a, 0x5, 0x44, 0x5, 0x78, 0x5, 0x40, 0x3, 0x80, 0x0
};

const unsigned char flappy2 [] = {
0x80, 0x3, 0x70, 0x4, 0x48, 0x4, 0x44, 0x4, 0x42, 0x6, 0x42, 0x9, 0x81, 0x8, 0x1, 0x8, 0x1d, 0x9, 0xa3, 0xa, 0x41, 0x5, 0x41, 0x5, 0x5a, 0x5, 0x44, 0x5, 0x78, 0x5, 0x40, 0x3, 0x80, 0x0
};

const unsigned char flappy3 [] = {
0x70, 0x0, 0x88, 0x0, 0x8, 0x3, 0xc, 0x5, 0xa, 0x5, 0x92, 0x8, 0x61, 0x8, 0x1, 0x8, 0x1d, 0x9, 0xa3, 0xa, 0x41, 0x5, 0x41, 0x5, 0x5a, 0x5, 0x44, 0x5, 0x78, 0x5, 0x40, 0x3, 0x80, 0x0
};

const unsigned char numbers [] = {
0x0, 0x7e, 0x42, 0x42, 0x7e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x0, 0x72, 0x52, 0x52, 0x5e, 0x0, 0x0, 0x42, 0x52, 0x52, 0x7e, 0x0, 0x0, 0x1e, 0x10, 0x10, 0x7e, 0x0, 0x0, 0x5e, 0x52, 0x52, 0x72, 0x0, 0x0, 0x7e, 0x52, 0x52, 0x72, 0x0, 0x0, 0x2, 0x2, 0x2, 0x7e, 0x0, 0x0, 0x7e, 0x52, 0x52, 0x7e, 0x0, 0x0, 0x1e, 0x12, 0x12, 0x7e, 0x0
};

const unsigned char pipe1 [] = {
0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x55, 0xaa, 0x55, 0x0, 0xff, 0x0, 0x0, 0x0
};

const unsigned char pipe2 [] = {
0x0, 0x3f, 0x21, 0xe1, 0x21, 0x21, 0x21, 0x21, 0x61, 0xab, 0x75, 0x2b, 0xf5, 0x21, 0x3f, 0x0
};

const unsigned char pipe3 [] = {
0x0, 0xfc, 0x84, 0x87, 0x84, 0x84, 0x84, 0x84, 0x85, 0xae, 0xd5, 0xac, 0xd7, 0x84, 0xfc, 0x0
};

unsigned char buffer1[LCD_WIDTH * LCD_HEIGHT8], buffer2[LCD_WIDTH * LCD_HEIGHT8];

void send(unsigned char type, unsigned char data);
void clean();
void bitmapFull(const char * bmp);
void bitmapSprite(const unsigned char * bmp, unsigned char w, unsigned char h, unsigned char x, unsigned char y);

// send a command or data to SPI
void send(unsigned char type, unsigned char data)
{
  // send command
	PORTD = CE_PIN | RE_PIN | type;

	PORTD &= ~CE_PIN;
  SPDR = data;
  while(!(SPSR & (1<<SPIF)));
	PORTD |= CE_PIN;
}

// send n chars to SPI with value data
void sendN(unsigned short n, unsigned char data)
{
  // send command
	PORTD = CE_PIN | RE_PIN | DATA;

	PORTD &= ~CE_PIN;
  for(;n;n--)
  {
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
  }
	PORTD |= CE_PIN;
}

// sends a buffer of size n to SPI
void sendBuffer(unsigned short n, const unsigned char * buf)
{
  unsigned char * b = (unsigned char *)buf;
  // send command
	PORTD = CE_PIN | RE_PIN | DATA;

	PORTD &= ~CE_PIN;
  for(;n;n--)
  {
    SPDR = *(b++);
    while(!(SPSR & (1<<SPIF)));
  }
	PORTD |= CE_PIN;
}

// clear the whole screen
void clean()
{
  send(CMD, 0x80);  // 0
  send(CMD, 0x40);  // 0

  sendN(LCD_WIDTH * (LCD_HEIGHT/8), 0x00);
}

void bitmapFull(const char * bmp)
{
  send(CMD, 0x80);
  send(CMD, 0x40);

  unsigned short i;

  for (i = 0; i < LCD_WIDTH * (LCD_HEIGHT/8); i++)
      send(DATA, bmp[i]);
}

void bitmapSprite(const unsigned char * bmp, unsigned char w, unsigned char h, unsigned char x, unsigned char y)
{
  h = h >> 3;
  y = y >> 3;

  const unsigned char * p = bmp;
  unsigned short xd, yd;

  for (yd = 0; yd < h; yd++)
  {
    send(CMD, 0x80 | x);
    send(CMD, 0x40 | y++);

    for (xd = 0; xd < w; xd++)
      send(DATA, pgm_read_byte(p++));
  }
}

void vSprite(const unsigned char * bmp, unsigned char w, unsigned char h, unsigned char x, unsigned char y, unsigned char d)
{
  y = y >> 3;

  const unsigned char * p = bmp;
  unsigned short xd, yd;
  unsigned char b;

  for (xd = 0; xd < w; xd++)
  {
    send(CMD, 0x80 | x++);
    send(CMD, 0x40 | y);

    for (yd = 0; yd < h; yd++)
    {
      b = *p << d;
      if (yd > 0)
        b|= *(p-1) >> (8 - d);
      send(DATA, b);
      p++;
    }

    if (d > 0) 
      send(DATA, *((p - h) - 1) >> (8 - d));
  }
}

void cleanFlappy(unsigned short y)
{
  y = y >> 3;

  send(CMD, 0x20); // horizontal

  send(CMD, 0x80 | 8);
  send(CMD, 0x40 | y);
  sendN(FLAPPY_WIDTH, 0x00);

  send(CMD, 0x80 | 8);
  send(CMD, 0x40 | (y+2));
  sendN(FLAPPY_WIDTH, 0x00);
}

void drawFlappy(unsigned short t, unsigned short y)
{
  const unsigned char * bmp;
  unsigned char ydisplacement;
  unsigned char p = (t) % 4;

  send(CMD, 0x22); // vertical

  switch(p)
  {
    case 0 :
      bmp = flappy1;
      break;
    case 1 :
      bmp = flappy2;
      break;
    case 2 :
      bmp = flappy1;
      break;
    case 3 :
      bmp = flappy3;
      break;
  }
  
  ydisplacement = y % 8;

  vSprite(bmp, FLAPPY_WIDTH, FLAPPY_HEIGHT, 8, y - ydisplacement, ydisplacement);
}

void drawPipes(unsigned char x, unsigned char y)
{

  unsigned char i;

  send(CMD, 0x20); // horizontal

  for (i = 0; i < y; i++)
  {
    send(CMD, 0x80 | x);
    send(CMD, 0x40 | i);
    sendBuffer(16, pipe1);
  }

  send(CMD, 0x80 | x);
  send(CMD, 0x40 | i);
  sendBuffer(16, pipe3);

  i += 3;

  send(CMD, 0x80 | x);
  send(CMD, 0x40 | i++);
  sendBuffer(16, pipe2);

  for (; i < 6; i++)
  {
    send(CMD, 0x80 | x);
    send(CMD, 0x40 | i);
    sendBuffer(16, pipe1);
  }
}

void drawGrass(unsigned short t)
{
  unsigned char x, g;

  send(CMD, 0x20); // horizontal

  send(CMD, 0x80);
  send(CMD, 0x40 | 5);

  for (x = LCD_WIDTH; x; x--)
  {
    t = t % 4;

    switch(t)
    {
      case 0 :
        g = 0x30;
        break;
      case 1 :
        g = 0x0c;
        break;
      case 2 :
        g = 0x30;
        break;
      case 3 :
        g = 0xc0;
        break;
    }

    send(DATA, g);
    t++;
  }
}

void drawScore(unsigned short score)
{
  unsigned char x;
  const unsigned char * nsprite;

  send(CMD, 0x20); // horizontal
  
  for(x = 6; score; score /= 10, x += 6)
  {
    send(CMD, 0x80 | (84 - x));
    send(CMD, 0x40);
    nsprite = numbers + (6 * (score % 10));

    sendBuffer(6, nsprite);
  }
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

  send(CMD, 0x21); //Tell LCD extended commands follow
  send(CMD, 0xB0); //Set LCD Vop (Contrast)
  send(CMD, 0x04); //Set Temp coefficent
  send(CMD, 0x13); //LCD bias mode 1:48 (try 0x13)

  // send(CMD, 0x20);
  send(CMD, 0x22); // vertical
  send(CMD, 0x0C); //Set display control, normal mode.

  clean();

  unsigned char flappy = 10;
  unsigned short t = 0;
  while(1) {
    cleanFlappy(flappy);

    if (PIND & BUTTON_PIN)
      flappy = (flappy > 0) ? flappy - 1 : 0;
    else
      flappy = (flappy < 32) ? flappy + 2 : 32;

    drawFlappy(t, flappy);
    drawPipes(68 - (t % 68), 0);
    drawGrass(t);
    drawScore(t);

    t++;
    _delay_ms(200);
  }
  
  return 0;
}



