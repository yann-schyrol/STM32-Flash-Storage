/*
 * BAENCODER.h
 *
 *  Created on: 14 août 2023
 *      Author: Yann Schyrol Kiti
 */

#ifndef BAENCODER_H_
#define BAENCODER_H_

#include <stdint.h>
#include <vector>
#include <functional>
#include "stm32f1xx_hal.h"
#include "BACOMMON.h"



class BA_ENCODER {
public:
	BA_ENCODER();
	~BA_ENCODER();
	/*static BA_ENCODER& getInstance(void) {
        static BA_ENCODER instance;
        return instance;
    }*/
	void initEncoder(GPIO_TypeDef *portA,uint16_t pinA, GPIO_TypeDef *portB,uint16_t pinB);

	using ForwardCallback = std::function<void()>;
	using BackwardCallback = std::function<void()>;

	uint32_t getMaxEncoderValue() const;
	void setMaxEncoderValue(uint32_t arg_u32MaxValue = 65535);

	uint32_t getMinEncoderValue() const;
	void setMinEncoderValue(uint32_t arg_u32MinValue = 0);

	uint16_t getIncrementStep() const;
	void setIncrementStep(uint16_t arg_u16IncrementStep = 0);

	uint32_t getCurrentEncoderValue() const;
	void setCurrentEncoderValue(const uint32_t arg_u32ActualEncoderValue);


	void setUserBackwardCallback(BackwardCallback userBackwardCallback);
	void setUserForwardCallback(ForwardCallback userForwardCallback);



	void HandleForward();
	void HandleBackward();
	void HandleGPIOEvents(uint16_t& which_pin); // used in GPIO_EXTI_CALLBack
	void cleanCount();
	uint32_t getCount() const;

private:

	GPIO_TypeDef *_portA;
	uint16_t _pinA;
	GPIO_TypeDef *_portB;
	uint16_t _pinB;


	uint8_t m_u8ForwardPreCounter = 0;
	uint8_t m_u8BackwardPreCounter = 0;
	uint32_t m_u32ActualEncoderValue = 0;
	uint32_t m_u32MaxEncoderValue = 65535;
	uint32_t m_u32MinEncoderValue = 0;
	uint16_t u16_IncrementStep{1};

	uint8_t compareByte[2];
	uint8_t forwardSteps = 0;
	uint8_t backwardSteps = 0;


	uint8_t m_bStateA = 0;
	uint8_t m_bStateB = 0;
	uint8_t m_bPrevStateA = 0;
	uint8_t m_bPrevStateB = 0;

	//uint8_t m_u8Step = 0;
	ForwardCallback UserForwardCallback;
	BackwardCallback UserBackwardCallback;

};

#endif /* BAENCODER_H_ */
