// Conways Game of Life
//   J3RE

#include "HaD_Badge.h"
#include "bh-badge-animate.h"

uint8_t countNeighbours(int8_t x, int8_t y)
{
  uint8_t counter = 0;

  for(int8_t i = -1; i <= 1; i++)
  {
    for(int8_t j = -1; j <= 1; j++)
    {
      if((i != 0) || (j != 0))
      {
        if(readPixel(x + i, y + j, Buffer))
          counter++;        
      }
    }
  }

  return counter;
}

void restart()
{  

  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = 0x00;
    // playground[y] = 0x00;
  }

  // do some random stuff
  uint8_t random = getTime() & 0xFF;
  uint32_t random2 = (random * random) ^ random << 4;

  playground[8] = random2 & 0xff;
  playground[9] = random & 0xff;


  playground[ 0] = (random + random2) ^ 0xfa;
  playground[ 1] = random - 73;
  playground[ 2] = random2 % 128;
  playground[ 3] = ((random2 >> 4) ^ (random)) & 0xff - random;
  playground[ 4] = random2 % 200;
  playground[ 5] = ((random2 >> 12) ^ random2) & 0xff;
  playground[ 6] = (random2 >> 8) & 0xff;
  playground[ 7] = random2 & 0xff;
  playground[ 8] = random & 0xff;
  playground[ 9] = (random ^ random2) & 0xff;
  playground[10] = playground[7] ^ playground[8];
  playground[11] = (~playground[4] + playground[8]) & 0xff;
  playground[12] = (random % 127 * random2  % 128) & 0xff;
  playground[13] = ~random2 % 255;
  playground[14] = ~(random2 >> 7) & 0xff;
  playground[15] = (playground[3] ^ playground[0]) & 0xff;


  // playground[ 0] = 0b00000000;
  // playground[ 1] = 0b00000000;
  // playground[ 2] = 0b00000000;
  // playground[ 3] = 0b00000000;
  // playground[ 4] = 0b00000000;
  // playground[ 5] = 0b00000000;
  // playground[ 6] = 0b00000000;
  // playground[ 7] = 0b00111000;
  // playground[ 8] = 0b00101000;
  // playground[ 9] = 0b00111000;
  // playground[10] = 0b00000000;
  // playground[11] = 0b00000000;
  // playground[12] = 0b00000000;
  // playground[13] = 0b00000000;
  // playground[14] = 0b00000000;
  // playground[15] = 0b10000000;

}

void liveOrDie()
{
  for(uint8_t x = 0; x < TOTPIXELX; x++)
  {
    for(uint8_t y = 0; y < TOTPIXELY; y++)
    {
      uint8_t neighbours = countNeighbours(x, y);
      if(readPixel(x, y, Buffer))
      {
        if ((neighbours < 2) || (neighbours > 3))
        {
          writePixel(x, y, 0, playground);
        }
      }
      else if(neighbours == 3)
      {
        writePixel(x, y, 1, playground);
      }
    }
  }
}  

void refreshMatrix()
{
  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = playground[y];
  }
  displayLatch();
}

uint8_t readPixel(int8_t x, int8_t y, uint8_t array[])
{
  if(!((x >= 0) && (x < TOTPIXELX) && (y >= 0) && (y < TOTPIXELY)))
    return 0;

  return (array[y] >> x) & 0x01;
}

void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[])
{
  if(live)
    array[y] |= (1 << x);
  else
    array[y] &= ~(1 << x);
}

void animateBadge(void)
{
  pause = 0;
  delay_time = 500;
 
  restart();

  while(1)
  {
    // show next generation
    refreshMatrix();     
    // calculate next generation
    liveOrDie();

    while( ((getTime() - time_old) < delay_time) || pause)
    {
      switch (getControl())
      {
        case (ESCAPE):
          displayClose();
          return;
        case (LEFT):
          if(delay_time > 50)
            delay_time -= 50;
          break;
        case (RIGHT):
          delay_time += 50;
          break;
        case (UP):
          restart();
          refreshMatrix();
          break;
        case (DOWN):
          pause = !pause;
          break;
      }
    }
    time_old = getTime();
  }

}
