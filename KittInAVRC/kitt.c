/*
 * Knight Rider. A shadowy flight into the dangerous world of a man
 * who does not exist. Michael Knight, a young loner on a crusade
 * to champion the cause of the innocent, the helpless, the powerless,
 * in a world of criminals who operate above the law.
 *
 * for arduino nano
 */

#include <avr/io.h>

int main(void)
{
  // arduino nano digital pins are made of RX/TX + 6bits at PORTD
  // plus 6bits at PORTB
  DDRD |= 0xF0;
  DDRB |= 0x3F;
  int i, j, k;

  int r = 0x1;
  int up = 1;

	while (1)
	{
    if (up)
    {
      if (r == 0x200)
      {
          up ^= 1;
          continue;
      }
      r <<= 1;
    }
    else
    {
      if (r == 1)
      {
          up ^= 1;
          continue;
      }
      r >>= 1;
    }
      
    PORTD = r << 4;
    PORTB = r >> 4;
    
    // sleep
    for(i = 0x0A; i > 0; i--)
      for(j = 0xFF; j > 0; j--)
        for(k = 0xFF; k > 0; k--)
          {}

	}
}
