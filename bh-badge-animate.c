// Conways Game of Life
//   J3RE

#include "HaD_Badge.h"
#include "bh-badge-animate.h"
#include "font.h"


typedef enum 
{
  PAUSE = 0,
  PLAY,
  MENU,
  EDITOR,
  PLAY_EDITOR,
  PAUSE_EDITOR
} mode_state;

mode_state mode = MENU;

uint8_t BufferBackup[TOTPIXELY];

uint16_t delay_time = 500;
uint16_t delay_time_scrolling = 100;
uint32_t time_old;
int8_t i_x = 0, i_y = 0;
uint8_t toggle_pixel = 0;

char text[]="Hackaday Belgrade!\0";
char char_play[]="play \0";
char char_editor[]="editor \0";

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

void restart(uint8_t array[])
{  

  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = 0x00;
    // array[y] = 0x00;
  }

  // do some random stuff
  uint8_t random = getTime() & 0xFF;
  uint32_t random2 = (random * random) ^ random << 4;

  array[8] = random2 & 0xff;
  array[9] = random & 0xff;


  array[ 0] = (random + random2) ^ 0xfa;
  array[ 1] = random - 73;
  array[ 2] = random2 % 128;
  array[ 3] = ((random2 >> 4) ^ (random)) & 0xff - random;
  array[ 4] = random2 % 200;
  array[ 5] = ((random2 >> 12) ^ random2) & 0xff;
  array[ 6] = (random2 >> 8) & 0xff;
  array[ 7] = random2 & 0xff;
  array[ 8] = random & 0xff;
  array[ 9] = (random ^ random2) & 0xff;
  array[10] = array[7] ^ array[8];
  array[11] = (~array[4] + array[8]) & 0xff;
  array[12] = (random % 127 * random2  % 128) & 0xff;
  array[13] = ~random2 % 255;
  array[14] = ~(random2 >> 7) & 0xff;
  array[15] = (array[3] ^ array[0]) & 0xff;


  // array[ 0] = 0b00000000;
  // array[ 1] = 0b00111000;
  // array[ 2] = 0b00000000;
  // array[ 3] = 0b00000000;
  // array[ 4] = 0b00000000;
  // array[ 5] = 0b00000000;
  // array[ 6] = 0b00000000;
  // array[ 7] = 0b00111000;
  // array[ 8] = 0b00101000;
  // array[ 9] = 0b00111000;
  // array[10] = 0b00000000;
  // array[11] = 0b00000000;
  // array[12] = 0b00000000;
  // array[13] = 0b00111000;
  // array[14] = 0b00000000;
  // array[15] = 0b00000000;

}

void liveOrDie(uint8_t array[])
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
          writePixel(x, y, 0, array);
        }
      }
      else if(neighbours == 3)
      {
        writePixel(x, y, 1, array);
      }
    }
  }
}  

void refreshMatrix(uint8_t array[])
{
  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = array[y];
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

void printChar(int8_t x, int8_t y, char character)
{

  uint16_t i_array = charToArray(character);

  for(int i = 0; i < 7; i++)
  {
    if(((y + i) < TOTPIXELY) && ((y+1) >= 0))
    {
      if(x <= 3)
        Buffer[y + i] |= font[i_array + i] << (3 - x);
      else
        Buffer[y + i] |= font[i_array + i] >> (x - 3);
    }
  }

  displayLatch();

}

// positive steps scroll left
void scrollDisplay(uint8_t y_start, uint8_t y_end, int8_t steps)
{
  if(y_end >= y_start)
  {
    for(int i = y_start; i <= y_end; i++)
    {
      if(steps > 0)
      {
        Buffer[i] <<= steps;
      }
      else
      {
        Buffer[i] >>= -steps;
      }
    }
  }
}

uint8_t checkButtons()
{
  uint8_t key_pressed = getControl();

  switch(mode)
  {
    case PAUSE:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          mode = MENU;
          displayClear();
          displayLatch();
          return 3;
          break;
        case RIGHT:
          mode = PLAY;
          break;
        case UP:
          return 2;
          break;
        case DOWN:
          mode = PLAY;
          break;
      }
      break;

    case PLAY:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          if(delay_time > 50)
          {
            delay_time -= 50;
            displayClear();
            printChar(2, 4, '-');
            time_old = getTime();
          }
          break;
        case RIGHT:
          delay_time += 50;
          displayClear();
          printChar(2, 4, '+');
          time_old = getTime();
          break;
        case UP:
          return 2;
          break;
        case DOWN:
          mode = PAUSE;
          break;
      }
      break;

    case MENU:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          delay_time_scrolling -= 10;
          break;
        case RIGHT:
          delay_time_scrolling += 10;
          break;
        case UP:
          mode = PLAY;
          return 2;
          break;
        case DOWN:
          mode = EDITOR;
          displayClear();
          displayLatch();
          break;
      }
      break;

    case EDITOR:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          toggle_pixel = 1;
          break;
        case RIGHT:
          if(++i_x == TOTPIXELX)
          {
            i_x = 0;
          }
          break;
        case UP:
          if(!i_x && !i_y)
          {
            i_y = -1;
          }
          else if(i_y)
          {
            i_y--;
          }
          break;
        case DOWN:
          if(i_y < (TOTPIXELY - 1))
          {
            i_y++;
          }
          break;
      }
      break;

    case PLAY_EDITOR:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          if(delay_time > 50)
          {
            delay_time -= 50;
            displayClear();
            printChar(2, 4, '-');
            time_old = getTime();
          }
          break;
        case RIGHT:
          delay_time += 50;
          displayClear();
          printChar(2, 4, '+');
          time_old = getTime();
          break;
        case UP:
          displayClear();
          for(int i = 0; i < TOTPIXELY; i++)
          {
            Buffer[i] = BufferBackup[i];
          }
          mode = EDITOR;
          break;
        case DOWN:
          mode = PAUSE_EDITOR;
          break;
      }
      break;

    case PAUSE_EDITOR:
      switch (key_pressed)
      {
        case ESCAPE:
          return 1;
        case LEFT:
          mode = MENU;
          displayClear();
          displayLatch();
          return 3;
          break;
        case RIGHT:
          mode = PLAY_EDITOR;
          break;
        case UP:
          return 2;
          break;
        case DOWN:
          mode = PLAY;
          break;
      }
      break;
  }

  return 0;
}

void animateBadge(void)
{ 
  uint8_t playground[TOTPIXELY];
  uint8_t pause_state = 0;
  uint8_t i_char1 = 0, i_char2 = 0;
  uint8_t x = 0, y = 0;

  restart(playground);

  while(1)
  {
    
    switch(mode)
    {
      case PAUSE:
        if((getTime() - time_old) >= 400)
        { 
          if(pause_state)
          {
            // show next generation
            refreshMatrix(playground);

            pause_state = 0;
          }
          else
          {
            displayClear();
            printChar(2, 4, 'P');

            pause_state = 1;
          }
          
          time_old = getTime();
        }
        break;

      case PLAY:
        if((getTime() - time_old) >= delay_time)
        { 
          // show next generation
          refreshMatrix(playground);     
          // calculate next generation
          liveOrDie(playground);
          
          time_old = getTime();
        }
        break;

      case MENU:

        if((getTime() - time_old) >= delay_time_scrolling)
        {
          i_x++;
          scrollDisplay(0, 15, 1);
          printChar(7 - i_x, 0, char_play[i_char1]);
          printChar(7 - i_x, 9, char_editor[i_char2]);
          
          if(i_x > 5)
          {
            i_x = 0;
            if(char_play[++i_char1] == '\0')
            {
              i_char1 = 0;
            }
            if(char_editor[++i_char2] == '\0')
            {
              i_char2 = 0;
            }
          }

          time_old = getTime();
        }
        break;

      case EDITOR:

        displayPixel(i_x, i_y, 1);

        if((i_x != x) || (i_y != y))
        {
          if(!toggle_pixel)
          {
            displayPixel(x, y, 0);
          }
          toggle_pixel = 0;
        }
        displayLatch();

        if(i_y == -1)
        {
          i_x = 0;
          i_y = 0;

          for(int i = 0; i < TOTPIXELY; i++)
          {
            BufferBackup[i] = Buffer[i];
            playground[i] = Buffer[i];
          }
          mode = PLAY_EDITOR;
        }
        x=i_x;
        y=i_y;
        break;

      case PLAY_EDITOR:
        if((getTime() - time_old) >= delay_time)
        { 
          // show next generation
          refreshMatrix(playground);     
          // calculate next generation
          liveOrDie(playground);
          
          time_old = getTime();
        }
        break;

      case PAUSE_EDITOR:
        if((getTime() - time_old) >= 400)
        { 
          if(pause_state)
          {
            // show next generation
            refreshMatrix(playground);

            pause_state = 0;
          }
          else
          {
            displayClear();
            printChar(2, 4, 'P');

            pause_state = 1;
          }
          
          time_old = getTime();
        }
        break;
    }

    switch(checkButtons())
    {
      case 1:
        displayClose();
        return;
        break;
      case 2:
        restart(playground);
        break;
    }
  }

}
