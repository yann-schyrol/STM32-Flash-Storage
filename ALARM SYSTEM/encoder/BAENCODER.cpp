/*
 * BAENCODER.cpp
 *
 *  Created on: 14 août 2023
 *      Author: Yann Schyrol Kiti
 */

#include "BAENCODER.h"

BA_ENCODER::BA_ENCODER()
{
	// TODO Auto-generated constructor stub
}

BA_ENCODER::~BA_ENCODER()
{
	// TODO Auto-generated destructor stub
}

void BA_ENCODER::initEncoder(GPIO_TypeDef *portA,uint16_t pinA, GPIO_TypeDef *portB,uint16_t pinB)
{
	_portA = portA;
	_portB = portB;
	_pinA = pinA;
	_pinB = pinB;


	m_bStateA  = HAL_GPIO_ReadPin(_portA,_pinA);
	m_bStateB  = HAL_GPIO_ReadPin(_portB,_pinB);
}

uint32_t BA_ENCODER::getMaxEncoderValue() const {
	return m_u32MaxEncoderValue;
}

void BA_ENCODER::setMaxEncoderValue(
		uint32_t arg_u32MaxValue) {
	m_u32MaxEncoderValue = arg_u32MaxValue;
}


uint32_t BA_ENCODER::getMinEncoderValue() const
{
	return m_u32MinEncoderValue;
}
void BA_ENCODER::setMinEncoderValue(uint32_t arg_u32MinValue)
{
	m_u32MinEncoderValue = arg_u32MinValue;
}

uint16_t BA_ENCODER::getIncrementStep() const
{
	return u16_IncrementStep;
}
void BA_ENCODER::setIncrementStep(uint16_t arg_u16IncrementStep)
{
	u16_IncrementStep = arg_u16IncrementStep;
}

uint32_t BA_ENCODER::getCurrentEncoderValue() const {
	return m_u32ActualEncoderValue;
}




void BA_ENCODER::setUserBackwardCallback(
		BackwardCallback userBackwardCallback) {
	UserBackwardCallback = userBackwardCallback;
}

void BA_ENCODER::setUserForwardCallback(ForwardCallback userForwardCallback) {
	UserForwardCallback = userForwardCallback;
}

void BA_ENCODER::HandleForward() {

	m_u32ActualEncoderValue += u16_IncrementStep;
	if(m_u32ActualEncoderValue > m_u32MaxEncoderValue)
	{
		m_u32ActualEncoderValue = m_u32MaxEncoderValue;
	}


	// execute forward callback
	if(UserForwardCallback){
		UserForwardCallback();
	}


}

void BA_ENCODER::HandleBackward() {



	if (m_u32ActualEncoderValue > u16_IncrementStep)
	{
		m_u32ActualEncoderValue -= u16_IncrementStep;
	}
	else{
		m_u32ActualEncoderValue = m_u32MinEncoderValue;
	}
	// execute backward callback
	if(UserBackwardCallback){
		UserBackwardCallback();
	}



}

void BA_ENCODER::HandleGPIOEvents(uint16_t& which_pin){


	/*
	 * How it works
	 * cw
	 *  A|B
	 *  ---
	 *  0|1 = 1
	 *  0|0 = 0
	 *  1|0 = 2
	 *  1|1 = 3
	 *
	 *  so state for CW 1 -> 0 -> 2 -> 3 -> 1 = cw direction
	 *
	 *  ccw
	 *  A|B
	 *  ---
	 *  1|0 = 2
	 *  0|0 = 0
	 *  0|1 = 1
	 *  1|1 = 3
	 *
	 *  so state for CCW 2 -> 0 -> 1 -> 3 -> 2 = ccw direction
	 *
	 */

	//store previous
	if(which_pin == _pinA || which_pin == _pinB){

		m_bPrevStateA = m_bStateA;
		m_bPrevStateB = m_bStateB;
		// store current
		if(HAL_GPIO_ReadPin(_portA,_pinA)){
			m_bStateA = 1;
		}
		else{
			m_bStateA = 0;
		}

		if( HAL_GPIO_ReadPin(_portB,_pinB)){
			m_bStateB = 1;
		}
		else{
			m_bStateB = 0;
		}

		compareByte[0] = 0xFF & ( (m_bPrevStateA<<1)|(m_bPrevStateB<<0) );

		compareByte[1] = 0xFF & ( (m_bStateA<<1)|(m_bStateB<<0) );

		if(compareByte[0] == 0){
			if(compareByte[1] == 2){
				forwardSteps++;
				backwardSteps=0;
			}
			else if(compareByte[1] == 1){
				backwardSteps++;
				forwardSteps = 0;
			}
		}

		else if (compareByte[0]==1){
			if(compareByte[1] == 0){
				forwardSteps++;
				backwardSteps=0;
			}
			else if(compareByte[1] == 3){
				backwardSteps++;
				forwardSteps=0;
			}
		}

		else if (compareByte[0] == 2) {
			if(compareByte[1] == 3){
				forwardSteps++;
				backwardSteps=0;
			}
			else if(compareByte[1] == 0){
				backwardSteps++;
				forwardSteps=0;
			}
		}

		else if (compareByte[0]  == 3) {
			if(compareByte[1] == 1){
				forwardSteps++;
				backwardSteps=0;
			}
			else if(compareByte[1] == 2){
				backwardSteps++;
				forwardSteps = 0;
			}
		}

		if(forwardSteps >= 3){
			HandleForward();
			forwardSteps = 0;
		}
		else if(backwardSteps >= 3){
			HandleBackward();
			backwardSteps=0;
		}
	}

}

//void BA_ENCODER::HandleGPIOEvents(uint16_t &which_pin) {
//
//	/*
//	 * Ref to page 617, Fig 169 of RM0090 Reference manual(stm32F40x programming manual) for info on Encoders
//	 */
//	if(which_pin==this->_pinA){
//
//
//		m_bStateA  = HAL_GPIO_ReadPin(_portA,_pinA);
//		m_bStateA =(m_eHardwareDefaultInputState == HW_INIT_STATE::PullUp)? !m_bStateA :m_bStateA ;
//
//		m_bPrevStateB = m_bStateB;
//		m_bStateB  = HAL_GPIO_ReadPin(_portB,_pinB);
//		m_bStateB =(m_eHardwareDefaultInputState == HW_INIT_STATE::PullUp)? !m_bStateB :m_bStateB ;
//		/*
//		 * Forward: 2 casses for A transitions
//		 * 1- A:L-H B: L-L ------------ I
//		 * 2- A:H-L, B:H-H -------------II
//		 */
//
//		/*
//		 * Backward: 2 Casses  for A transition
//		 * 1- A:L-H B:H-H ------------ III
//		 * 2- A:H-L B:L-L ------------ IV
//		 */
//
//		if(m_bStateA != m_bPrevStateA  ){
//			if(m_bPrevStateA==false && m_bStateA==true){
//				if(m_bPrevStateB==false && m_bStateB==false){
//					//forward --------- I
//					HandleForward();
//				}
//				if(m_bPrevStateB==true && m_bStateB==true){
//					//backward ---------III
//					HandleBackward();
//
//				}
//
//			}
//			m_bPrevStateA = m_bStateA;
//		}
//
//		/*
//		 * Tested, only one condition works fine
//		 */
//
//
//		/*if(m_bPrevStateA==true && m_bStateA==false){
//
//			if(m_bPrevStateB==true && m_bStateB==true){
//				//forward --------- II
//				HandleForward();
//			}
//
//			if(m_bPrevStateB==true && m_bStateB==true){
//				//backward --------- IV
//				HandleBackward();
//			}
//
//
//		}*/
//
//	}
//	else if(which_pin==this->_pinB){
//
//		m_bPrevStateB = m_bStateB;
//		m_bStateB  = HAL_GPIO_ReadPin(_portB,_pinB);
//		m_bStateB =(m_eHardwareDefaultInputState == HW_INIT_STATE::PullUp)? !m_bStateB :m_bStateB ;
//
//		m_bPrevStateA = m_bStateA;
//		m_bStateA  = HAL_GPIO_ReadPin(_portA,_pinA);
//		m_bStateA =(m_eHardwareDefaultInputState == HW_INIT_STATE::PullUp)? !m_bStateA :m_bStateA ;
//
//
//		/*
//		 * Forward: 2 caseses for B transitions
//		 * 1- B:L-H A: H-H -------- I
//		 * 2- B:H-L, A:L-L ---------II
//		 */
//
//		/*
//		 * Backward: 2 cases for B transitions
//		 * 1- B:L-H A:L-L ------- III
//		 * 2- B:H-L A:H-H ------- IV
//		 */
//		if(m_bStateB != m_bPrevStateB){
//			if(m_bPrevStateB==false &&  m_bStateB==true){
//				if(m_bPrevStateA==true && m_bStateA==true){
//					//forward -------- I
//					HandleForward();
//				}
//				if(m_bPrevStateA==false && m_bStateA== false){
//					//Backward -------- III
//					HandleBackward();
//				}
//
//			}
//		}
//		/*
//		 * Tested, only one condition works fine
//		 */
//
//		/*if(m_bPrevStateB==true &&  m_bStateB==false){
//			if(m_bPrevStateA==false && m_bStateA==false){
//				//forward -------- II
//				HandleForward();
//			}
//			if(m_bPrevStateA==true && m_bStateA== true){
//				//Backward -------- IV
//				HandleBackward();
//			}
//
//		}*/
//
//
//	}
//
//}

void BA_ENCODER::cleanCount() {
	m_u32ActualEncoderValue=0;
}

void BA_ENCODER::setCurrentEncoderValue(
		const uint32_t arg_u32ActualEncoderValue) {
	m_u32ActualEncoderValue = arg_u32ActualEncoderValue;
}

uint32_t BA_ENCODER::getCount() const {
	return m_u32ActualEncoderValue;
}
