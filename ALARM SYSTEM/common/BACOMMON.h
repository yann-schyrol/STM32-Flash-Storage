/*
 * BA_common.h
 *
 *  Created on: 14 août 2023
 *      Author: Yann Schyrol Kiti
 */

#ifndef BACOMMON_H_
#define BACOMMON_H_
#include "string"

typedef enum {

	PullDown = 0,
	PullUp
}HW_INIT_STATE;

typedef enum {

	CW = 0,
	CCW

}Direction;

typedef enum {
	US = 0,
	MS
}TimeFrameScale;


typedef struct{

	uint32_t min;
	uint32_t max;

	const char* code;
	uint32_t value_milli;
	const char* unit;

	uint16_t rendering;//1000
	uint16_t stepIncrease;

}Parameter;


typedef struct IhmMessages
{
	char message[15];
	uint8_t u8CoordX;
	uint8_t u8CoordY;
	//FontDef sFont;
	bool btoDisplay;
}Ts_OledMessage;



#endif /* BACOMMON_H_ */
