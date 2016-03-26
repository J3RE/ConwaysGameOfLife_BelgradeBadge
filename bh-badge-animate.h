#include "HaD_Badge.h"

uint8_t playground[TOTPIXELY];
uint8_t pause;
uint32_t delay_time;
uint32_t time_old;

//Add your function prototypes:
uint8_t readPixel(int8_t x, int8_t y, uint8_t array[]);
void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[]);
uint8_t countNeighbours(int8_t x, int8_t y);
void liveOrDie();
void restart();
void refreshMatrix();

//This function prototype needs to be here:
void animateBadge(void);

