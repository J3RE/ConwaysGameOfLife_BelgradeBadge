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

char intro[]="Hackaday Belgrade! \0";
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
}

uint8_t readPixel(int8_t x, int8_t y, uint8_t array[])
{
  if(!((x >= 0) && (x < TOTPIXELX) && (y >= 0) && (y < TOTPIXELY)))
    return 0;

  return (array[y] >> (7 - x)) & 0x01;
}

void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[])
{
  if(live)
    array[y] |= (1 << (7 - x));
  else
    array[y] &= ~(1 << (7 - x));
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

void animateBadge(void)
{ 
  mode_state mode = MENU;

  uint8_t playground[TOTPIXELY];
  uint8_t BufferEditor[TOTPIXELY] = {};
  
  uint8_t pause_state = 0, blink_editor = 0;
  uint8_t i_char1 = 0, i_char2 = 0;
  int8_t i_x = 0, i_y = 0;
  uint32_t time_old;
  uint16_t delay_time = 500;
  uint16_t delay_time_scrolling = 100;

  // add a smiley, Hackaday logo is to complex
  Buffer[ 8] = 0b00111100;
  Buffer[ 9] = 0b01000010;
  Buffer[10] = 0b10100101;
  Buffer[11] = 0b10000001;
  Buffer[12] = 0b10100101;
  Buffer[13] = 0b10011001;
  Buffer[14] = 0b01000010;
  Buffer[15] = 0b00111100;

  while(getControl() == NOINPUT)
  {
    if((getTime() - time_old) >= delay_time_scrolling)
    {
      i_x++;
      scrollDisplay(0, 7, 1);
      printChar(7 - i_x, 0, intro[i_char1]);
      
      if(i_x > 5)
      {
        i_x = 0;
        if(intro[++i_char1] == '\0')
        {
          i_char1 = 0;
        }
      }
  
      displayLatch();
      time_old = getTime();
    }
  }
  displayClear();
  displayLatch();
  
  i_char1 = 0;

  while(1)
  {
    uint8_t key_pressed = getControl();
    
    switch(mode)
    {
      case PAUSE:
        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            mode = MENU;
            i_char1 = 0;
            i_char2 = 0;
            displayClear();
            displayLatch();
            break;
          case RIGHT:
            break;
          case UP:
            restart(playground);
            break;
          case DOWN:
            mode = PLAY;
            break;
        }

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

          displayLatch();
          time_old = getTime();
        }
        break;

      case PLAY:
        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            if(delay_time > 50)
            {
              delay_time -= 50;
              displayClear();
              printChar(2, 4, '-');
              displayLatch();
              time_old = getTime();
            }
            break;
          case RIGHT:
            delay_time += 50;
            displayClear();
            printChar(2, 4, '+');
            displayLatch();
            time_old = getTime();
            break;
          case UP:
            restart(playground);
            break;
          case DOWN:
            mode = PAUSE;
            break;
        }

        if((getTime() - time_old) >= delay_time)
        { 
          // show next generation
          refreshMatrix(playground);     
          // calculate next generation
          liveOrDie(playground);


          displayLatch();
          time_old = getTime();
        }
        break;

      case MENU:
        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            delay_time_scrolling -= 10;
            break;
          case RIGHT:
            delay_time_scrolling += 10;
            break;
          case UP:
            mode = PLAY;
            restart(playground);
            break;
          case DOWN:
            mode = EDITOR;
            i_x = 0;
            i_y = 0;
            displayClear();
            displayLatch();
            break;
        }

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

          displayLatch();
          time_old = getTime();
        }
        break;

      case EDITOR:

        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            // toggle Pixel
            if(readPixel(i_x, i_y, BufferEditor))
            {
              writePixel(i_x, i_y, 0, BufferEditor);
            }
            else
            {
              writePixel(i_x, i_y, 1, BufferEditor);
            }
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
              refreshMatrix(BufferEditor);
              displayLatch();
              for(int i = 0; i < TOTPIXELY; i++)
              {
                playground[i] = BufferEditor[i];
              }
              mode = PLAY_EDITOR;
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

        if((getTime() - time_old) >= 100)
        { 
          refreshMatrix(BufferEditor);

          if(!blink_editor)
          {
            displayPixel(i_x, i_y, 1);
          }
          else if((blink_editor == 1) && !readPixel(i_x, i_y, BufferEditor))
          {
            displayPixel(i_x, i_y, 0); 
          }
          else if (blink_editor == 3)
          {
            displayPixel(i_x, i_y, 0);
          }
          blink_editor++;

          if(blink_editor >= 4)
          {
            blink_editor = 0;
          }

          displayLatch();
          
          time_old = getTime();
        }
        break;

      case PLAY_EDITOR:

        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            if(delay_time > 50)
            {
              delay_time -= 50;
              displayClear();
              printChar(2, 4, '-');
              displayLatch();
              time_old = getTime();
            }
            break;
          case RIGHT:
            delay_time += 50;
            displayClear();
            printChar(2, 4, '+');
            displayLatch();
            time_old = getTime();
            break;
          case UP:
            displayClear();
            displayLatch();
            for(int i = 0; i < TOTPIXELY; i++)
            {
              Buffer[i] = BufferEditor[i];
            }
            mode = EDITOR;
            i_x = 0;
            i_y = 0;
            break;
          case DOWN:
            mode = PAUSE_EDITOR;
            break;
        }

        if((getTime() - time_old) >= delay_time)
        { 
          // show next generation
          refreshMatrix(playground);     
          // calculate next generation
          liveOrDie(playground);
          
          displayLatch();
          time_old = getTime();
        }
        break;

      case PAUSE_EDITOR:

        switch (key_pressed)
        {
          case ESCAPE:
            displayClose();
            return;
          case LEFT:
            mode = MENU;
            i_char1 = 0;
            i_char2 = 0;
            displayClear();
            displayLatch();
            break;
          case RIGHT:
            break;
          case UP:
            displayClear();
            displayLatch();
            for(int i = 0; i < TOTPIXELY; i++)
            {
              Buffer[i] = BufferEditor[i];
            }
            mode = EDITOR;
            i_x = 0;
            i_y = 0;
            break;
          case DOWN:
            mode = PLAY_EDITOR;
            break;
        }

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
          
          displayLatch();
          time_old = getTime();
        }
        break;
    }
  }

}
