#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "main.h"
#include <math.h>

typedef struct Accelerometer_Result {
	double angle;
	long deviation;
	int8_t x;
	int8_t y;
	int8_t z;
} Accelerometer_Result;

void accelerometer_Init(void);
void accelerometer_SendData(uint8_t adress, uint8_t data);
uint8_t accelerometer_GetData(uint8_t adress);
Accelerometer_Result accelerometer_Work(void);

#endif
