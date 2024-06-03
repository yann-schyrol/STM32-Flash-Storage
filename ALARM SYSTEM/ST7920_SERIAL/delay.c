/*
 * delay.c
 *
 *  Created on: Apr 24, 2024
 *      Author: Admin
 */



#include "delay.h"
#include "stm32f1xx.h"

extern TIM_HandleTypeDef htim2;


void delay_init ()
{
	HAL_TIM_Base_Start(&htim2);  // change this according to ur setup
}

void delay_us (uint16_t delay)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);  // reset the counter
	while ((__HAL_TIM_GET_COUNTER(&htim2))<delay);  // wait for the delay to complete
}
