// Conways Game of Life
//   J3RE

#include "HaD_Badge.h"
#include "bh-badge-animate.h"
#include "font.h"

typedef enum 
{
  MENU = 0,
  PLAY,
  PAUSE,
  EDITOR,
  PLAY_EDITOR,
  PAUSE_EDITOR
} mode_state;

// returns the active pixels around pixel(@x,@y)
uint8_t countNeighbours(int8_t x, int8_t y)
{
  uint8_t counter = 0;

  for(int8_t i = -1; i <= 1; i++)
  {
    for(int8_t j = -1; j <= 1; j++)
    {
      // you are not a neighbour of yourself
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

  // clear the matrix
  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = 0x00;
  }

  // do some "random" stuff
  uint8_t random = getTime() & 0xFF;
  uint32_t random2 = (random * random) ^ random << 4;

  array[ 0] = (random + random2) ^ 0xfa;
  array[ 1] = random - 73;
  array[ 2] = random2 % 128;
  array[ 3] = (((random2 >> 4) ^ (random)) - random2) & 0xff;
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

// calculate the next generation
void liveOrDie(uint8_t array[])
{
  for(uint8_t x = 0; x < TOTPIXELX; x++)
  {
    for(uint8_t y = 0; y < TOTPIXELY; y++)
    {
      // get neighbours of a pixel
      uint8_t neighbours = countNeighbours(x, y);
      if(readPixel(x, y, Buffer))
      {
        // if a pixel is alive and has less than 2 or more than 3 neighbours, kill it
        if ((neighbours < 2) || (neighbours > 3))
        {
          writePixel(x, y, 0, array);
        }
      }
      else if(neighbours == 3)
      {
        // if an pixel is dead and has 3 neighbours revive it
        writePixel(x, y, 1, array);
      }
    }
  }
}  

// copy an @array to the @Buffer
// the @Buffer is displayed by the LED Matrix
void refreshMatrix(uint8_t array[])
{
  for(uint8_t y = 0; y < TOTPIXELY; y++)
  {
    Buffer[y] = array[y];
  }
}

// read the state of the pixel(@x,@y) in the @array
uint8_t readPixel(int8_t x, int8_t y, uint8_t array[])
{
  // if x or y is out of bounds return 0
  if(!((x >= 0) && (x < TOTPIXELX) && (y >= 0) && (y < TOTPIXELY)))
    return 0;

  return (array[y] >> (7 - x)) & 0x01;
}

// write @live to the pixel(@x,@y) of the @array
void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[])
{
  if(live)
    array[y] |= (1 << (7 - x));
  else
    array[y] &= ~(1 << (7 - x));
}

// print a character to the @Buffer at position @x,@y
void printChar(int8_t x, int8_t y, char character)
{

  uint16_t i_array = charToArray(character);

  for(int i = 0; i < 7; i++)
  {
    if(((y + i) < TOTPIXELY) && ((y + i) >= 0))
    {
      // shift the @character to the right position
      if(x <= 3)
        Buffer[y + i] |= font[i_array + i] << (3 - x);
      else
        Buffer[y + i] |= font[i_array + i] >> (x - 3);
    }
  }

  displayLatch();

}

// shifts the Buffer rows @y_start - @y_end by #@steps
void scrollDisplay(uint8_t y_start, uint8_t y_end, int8_t steps)
{
  // check for valid input
  if(y_end >= y_start)
  {
    for(int i = y_start; i <= y_end; i++)
    {
      // positive steps scroll left
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

// increase or decrease the @delay_time
// display the current @delay_time in a bargraph-style at the first column
void changeDelaytime(uint16_t *delay_time, uint16_t *timeout, int8_t increase)
{
  // display the delay_time-screen for 700ms
  *timeout = 700;

  displayClear();

  // in- or decrease the @delay_time by 50ms
  if(increase)
  {
    if(*delay_time < 1650)
    {
      *delay_time += 50;
      printChar(2, 4, '+');
      // map the @delay_time to 16 pixels
      for(uint16_t i = 0; i < *delay_time / 100; i++)
      {
        displayPixel(0, 15 - i, 1);
      }

      displayLatch();
      return;
    }
  }
  else
  {           
    if(*delay_time > 50)
    {
      *delay_time -= 50;
      printChar(2, 4, '-');
      // map the @delay_time to 16 pixels
      for(uint16_t i = 0; i < *delay_time / 100; i++)
      {
        displayPixel(0, 15 - i, 1);
      }

      displayLatch();
      return;
    }
  }
  
  // if the @delay_time is at max or min, show the equal-sign and the delay_time-bargraph
  printChar(2, 4, '=');
  for(uint16_t i = 0; i < *delay_time / 100; i++)
  {
    displayPixel(0, 15 - i, 1);
  }

  displayLatch();
}

void animateBadge(void)
{
  // start in the menu
  mode_state mode = MENU;

  char intro[]="Hackaday Belgrade! \0";
  char char_play[]="play \0";
  char char_editor[]="editor \0";

  uint8_t playground[TOTPIXELY];
  uint8_t BufferEditor[TOTPIXELY] = {};
  
  uint8_t pause_state, blink_editor;
  uint8_t i_char1 = 0, i_char2 = 0;
  int8_t i_x = 0, i_y = 0;

  uint16_t timeout = 0;
  uint16_t delay_time = 500;
  uint16_t delay_time_scrolling = 100;
  uint32_t time_old;

  // add a smiley, Hackaday logo is to complex
  Buffer[ 8] = 0b00111100;
  Buffer[ 9] = 0b01000010;
  Buffer[10] = 0b10100101;
  Buffer[11] = 0b10000001;
  Buffer[12] = 0b10100101;
  Buffer[13] = 0b10011001;
  Buffer[14] = 0b01000010;
  Buffer[15] = 0b00111100;

  // start with an intro screen
  while(getControl() == NOINPUT)
  {
    // display a scrolling text
    if((getTime() - time_old) >= delay_time_scrolling)
    {
      // shift the first 8 rows to the left by 1
      scrollDisplay(0, 7, 1);
      printChar(7 - i_x, 0, intro[i_char1]);
      i_x++;
      
      if(i_x > 5)
      {
        i_x = 0;
        if(intro[++i_char1] == '\0')
        {
          // start again at the beginning
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

        // toggle between the @playground and a 'P'-character (for pause)
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
            changeDelaytime(&delay_time, &timeout, 0);
            break;
          case RIGHT:
            changeDelaytime(&delay_time, &timeout, 1);
            break;
          case UP:
            restart(playground);
            break;
          case DOWN:
            mode = PAUSE;
            break;
        }

        if((getTime() - time_old) >= (delay_time + timeout))
        {
          timeout = 0; 
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
            if(delay_time_scrolling > 20)
            {
              delay_time_scrolling -= 20;
            }
            break;
          case RIGHT:
            if(delay_time_scrolling < 500)
            {
              delay_time_scrolling += 20;
            }
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

        // scroll two strings at the same time
        if((getTime() - time_old) >= delay_time_scrolling)
        {
          scrollDisplay(0, 15, 1);
          printChar(7 - i_x, 0, char_play[i_char1]);
          printChar(7 - i_x, 9, char_editor[i_char2]);
          i_x++;
          
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

        // change the duty cycle of the selected pixel according to it's state
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
            changeDelaytime(&delay_time, &timeout, 0);
            break;
          case RIGHT:
            changeDelaytime(&delay_time, &timeout, 1);
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

        if((getTime() - time_old) >= (delay_time + timeout))
        {
          timeout = 0; 
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

        // toggle between the @playground and a 'P'-character (for pause)
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
