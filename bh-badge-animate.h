#include "HaD_Badge.h"

// function prototypes
uint8_t readPixel(int8_t x, int8_t y, uint8_t array[]);
void writePixel(uint8_t x, uint8_t y, uint8_t live, uint8_t array[]);
uint8_t countNeighbours(int8_t x, int8_t y);
void liveOrDie(uint8_t array[]);
void restart(uint8_t array[]);
void refreshMatrix(uint8_t array[]);
void printChar(int8_t x, int8_t y, char character);
void scrollDisplay(uint8_t y_start, uint8_t y_end, int8_t steps);
void animateBadge(void);