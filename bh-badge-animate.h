#include "HaD_Badge.h"

//Add your function prototypes:
uint8_t readPixel(int8_t x, int8_t y, uint8_t array[]);
void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[]);
uint8_t countNeighbours(int8_t x, int8_t y);
void liveOrDie();
void restart();
void refreshMatrix();
uint8_t checkButtons();
void printChar(int8_t x, int8_t y, char character);
void scrollDisplay(uint8_t y_start, uint8_t y_end, int8_t steps);

//This function prototype needs to be here:
void animateBadge(void);

