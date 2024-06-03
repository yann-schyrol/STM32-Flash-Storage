/*
 * delay.h
 *
 *  Created on: Apr 24, 2024
 *      Author: Admin
 */

#ifndef DELAY_H_
#define DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void delay_init ();

void delay_us (uint16_t delay);

#ifdef __cplusplus
}
#endif

#endif /* DELAY_H_ */
