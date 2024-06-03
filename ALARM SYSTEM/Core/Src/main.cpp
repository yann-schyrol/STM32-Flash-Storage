/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program bodyp
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BACOMMON.h"
#include "BAENCODER.h"
#include "delay.h"
#include "ST7920_SERIAL.h"
#include "BA_bitmap.h"
#include "DataMapping.h"
#include "flash.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum eMainMode {
	MAIN_HOME = 0,
	MAIN_MENU,
	MAIN_PARAMETERS

} Ts_MainMode;

typedef enum eStateMode {
	STATE_RUN=0,
	STATE_ALARM,
	STATE_WAIT

} Ts_StateMode;

typedef enum eKeyPress {
	KEY_NONE = 0,
	KEY_RESET,
	KEY_MODE,
	KEY_ESC,
	KEY_ENT

} Ts_KeyPress;

typedef enum DataIndex {
	F_STATE = 0,
	F_TDETECT,
	F_TWAIT

}Ts_FlashDataIndex;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_PRESET_TIME  120000 //2min
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BA_ENCODER oJog;
//Parameter detectTime;
//Parameter waitTime;
Parameter parameter[2];
uint8_t detected = 0;

Ts_KeyPress KeyPress_t = KEY_NONE;
Ts_StateMode StateMode_t = STATE_RUN;
Ts_MainMode MainMode_t = MAIN_HOME;

Parameter *currentParameter;
const char d[] = {'0','1','2','3','4','5','6','7','8','9'};
uint8_t init_run = 0;
uint8_t init_alarm = 0;
uint8_t init_wait = 0;
char remainingTime[9] = {'0','0',':','0','9',':','5','9','\0'};
volatile uint32_t remainingTimeValue =0;
uint32_t initialTimeValue =0;

volatile uint8_t light_Display = 0;
volatile uint8_t light_Display_on = 0;

static volatile uint32_t holdCurrenTimeCount = 0;


volatile uint32_t state_Machine_previousTime = 0;
uint32_t state_Machine_presetTime = 0;

uint32_t LCD_previousTime = 0;
uint32_t update_previousTime = 0;

uint8_t setting_animation = 0;
uint8_t home_animation = 0;
uint8_t save = 0;
uint32_t holdParameterValue = 0;
uint8_t clear_screen = 0;

uint32_t toFlashData[3]={0};
uint16_t address_offset_toFlashData[3]= {0,4,8}; // eg x08000000 + address_offset_toFlashData[0];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Paramertes_Init(void);
void DisplayManagement(void);
void StateMachine(void);
void Encoder_Init(void);

void RUN(void);
void ALARM(void);
void WAIT(void);

void detect(void);
void reinitVariables(void);
void checkActivateScreen(void);

inline uint32_t currentTime(void);


void logoAnimation(void);

void drawHomePage(uint8_t state,  int percent);
void drawSettingsPage(uint8_t state);
void drawParameterPage(void);

void progressBar(int x, int y, int h, int percentage){
	int l = 50;
	char p[5];
	DrawRectangle(x, y, l, h);
	DrawFilledRectangle(x, y, (l*percentage)/100, h);
	ST7920_SendString(x/16, l/8 + 1, p);
}


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Paramertes_Init(void){

	//if memory is zero, Load
	//else init
	parameter[0].code = "Tdtct";
	parameter[0].max = 7200;//2h min
	parameter[0].min = 10;
	parameter[0].rendering = 1;
	parameter[0].stepIncrease = 10;
	parameter[0].unit = "sec";
	parameter[0].value_milli = 60;


	parameter[1].code = "Twait";
	parameter[1].max = 7200;
	parameter[1].min = 10;
	parameter[1].rendering = 1;
	parameter[1].stepIncrease = 10;
	parameter[1].unit = "sec";
	parameter[1].value_milli = 60;



	currentParameter = &parameter[0];
}
void DisplayManagement(){

	//main screen
	//Menu display
	//parametere display
	if(clear_screen){

		ST7920_Init();
		ST7920_Clear();

		clear_screen = 0;
	}


	int  ptage =0;

	if(initialTimeValue >0)
		ptage = remainingTimeValue*100/initialTimeValue;

	//split time
	int hh = (remainingTimeValue/3600);
	int mm = (remainingTimeValue%3600)/60;

	int ss = (remainingTimeValue%3600)%60;

	remainingTime[0] =d[hh/10];
	remainingTime[1] =d[hh%10];
	remainingTime[2] =':';
	remainingTime[3] =d[mm/10];
	remainingTime[4] =d[mm%10];
	remainingTime[5] =':';
	remainingTime[6] =d[ss/10];
	remainingTime[7] =d[ss%10];
	remainingTime[8] ='\0';

	switch (MainMode_t) {
	case MAIN_HOME:
		home_animation = (uint8_t)StateMode_t;
		drawHomePage(home_animation,  ptage);
		break;
	case MAIN_MENU:
		setting_animation = oJog.getCurrentEncoderValue();
		drawSettingsPage(setting_animation);
		break;

	case MAIN_PARAMETERS:
		holdParameterValue = oJog.getCurrentEncoderValue();
		if(holdParameterValue<currentParameter->min){
			holdParameterValue = currentParameter->min;
		}
		drawParameterPage();
		break;
	}

}
void StateMachine(){
	//
	switch(StateMode_t){

	break;
	case STATE_RUN:
		RUN();
		break;
	case STATE_ALARM:
		ALARM();
		break;
	case STATE_WAIT:
		WAIT();
		break;

	}
}

void RUN(void){
	//ON the green LED
	if(!init_run){
		// light green lamp only when there is detection
		reinitVariables();

		//state_Machine_presetTime = parameter[0].value_milli*1000;
		state_Machine_previousTime = currentTime();
		initialTimeValue =  parameter[0].value_milli;
		remainingTimeValue = initialTimeValue;
		init_run = 1;

		toFlashData[F_STATE] = 0;
		save = 1;

	}
	else{

		if(!remainingTimeValue ){

			//pass to other state
			StateMode_t = STATE_ALARM;

			reinitVariables();
		}
	}


}
void ALARM(void){
	if(!init_alarm){

		reinitVariables();
		remainingTimeValue = 0;
		init_alarm = 1;
		HAL_GPIO_WritePin(RC2_GPIO_Port, RC2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RC3_GPIO_Port, RC3_Pin, GPIO_PIN_SET);
		state_Machine_previousTime = currentTime();
		toFlashData[F_STATE] = 1;
		save = 1;
	}
	/*else{
		// blink RC2
		if((currentTime() - state_Machine_previousTime) >= 1000){
			HAL_GPIO_TogglePin(RC2_GPIO_Port, RC2_Pin);
			state_Machine_previousTime = currentTime();
		}
	}*/


}
void WAIT(void){

	if(!init_wait){
		reinitVariables();

		//HAL_GPIO_WritePin(RC2_GPIO_Port, RC2_Pin, GPIO_PIN_SET);
		//state_Machine_presetTime = parameter[1].value_milli*1000;
		initialTimeValue =  parameter[1].value_milli;
		remainingTimeValue = initialTimeValue;
		init_wait = 1;

		toFlashData[F_STATE] = 2;
		save = 1;

		state_Machine_previousTime = currentTime();
	}else{



		if(!remainingTimeValue ){

			//pass to other state
			StateMode_t = STATE_RUN;

			reinitVariables();
		}
	}

}

void detect(){
	// a detection of input will take any othe state to the runing state and reinitilize all times

	if(detected){


		reinitVariables();

		HAL_GPIO_WritePin(RC1_GPIO_Port, RC1_Pin, GPIO_PIN_SET);
		StateMode_t = STATE_RUN;
		detected = 0;
	}
}
void reinitVariables(){
	HAL_GPIO_WritePin(RC1_GPIO_Port, RC1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RC2_GPIO_Port, RC2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RC3_GPIO_Port, RC3_Pin, GPIO_PIN_RESET);

	init_run = 0;
	init_alarm = 0;
	init_wait = 0;

	state_Machine_previousTime = currentTime();

}
void checkActivateScreen(){

	if(light_Display){
		HAL_GPIO_WritePin(LED_DISP_GPIO_Port, LED_DISP_Pin, GPIO_PIN_SET);
		LCD_previousTime = currentTime();
		light_Display = 0;
		light_Display_on = 1;
	}
	if(  (currentTime() - LCD_previousTime) >= LCD_PRESET_TIME ){
		HAL_GPIO_WritePin(LED_DISP_GPIO_Port, LED_DISP_Pin, GPIO_PIN_RESET);
		light_Display_on = 0;
	}
}

inline uint32_t currentTime(){
	return holdCurrenTimeCount;
}

void logoAnimation(){
	delay_init();

	ST7920_Init();

	ST7920_GraphicMode(1);
	ST7920_DrawBitmap(bana_logo);
	HAL_Delay(1000);

	DrawFilledRectangle(0, 0, 128, 64);
	ST7920_Update();
	HAL_Delay(70);
	ST7920_GraphicMode(0);
	ST7920_SendString(1,1, "L'INNOVATION");
	ST7920_SendString(2,0, "N'A PAS DE FREIN");
	ST7920_scroll(3, "www.banaafrica.com", 1, 150);
	HAL_Delay(70);
	ST7920_Clear();
	HAL_Delay(100);
}

void drawHomePage(uint8_t state, int percent){
	// 1- draw backbone

	ST7920_ClearBuffer();
	ST7920_GraphicMode(1);

	//draw low rectangles
	DrawFilledRectangle(0, 48, 40, 15);
	DrawFilledRectangle(44, 48, 40, 15);
	DrawFilledRectangle(87, 48, 40, 15);

	DrawLine(87, 0, 87, 47);// vertical line

	DrawLine(87, 15, 127, 15);// H line 1
	DrawLine(87, 31, 127, 31);// H line 2

	DrawLine(0, 0, 0, 47);// vertical line2
	DrawLine(86, 0, 86, 47);// vertical line3

	DrawLine(0, 0, 127,0);// H line 3
	DrawLine(0, 47, 86, 47);// H line 4


	// 2- make right state

	if (state%3==0){
		DrawFilledRectangle(87, 0, 42, 15);
	}
	else if(state%3 == 1){
		DrawFilledRectangle(87, 16, 42, 15);
	}
	else if(state%3 == 2){
		DrawFilledRectangle(87, 31, 42, 15);
	}

	//progress bar
	progressBar(5, 35, 8, percent);

	ST7920_Update();

	// 3- put text

	ST7920_GraphicMode(0);
	ST7920_SendString(0,0, "Time Left" );//CDT: count Down Time
	//clear
	ST7920_SendString(1,0, "                ");
	ST7920_SendString(1,0, remainingTime);


	ST7920_SendString(0,6, "RUN");
	ST7920_SendString(1,6, "ALRM");
	ST7920_SendString(2,6, "WAIT");

	//give possible actions
	ST7920_SendString(3,0, "MENU");
	ST7920_SendString(3,3, "RST");
	ST7920_SendString(3,6, "ESC");

}
void drawSettingsPage(uint8_t state){
	// 1- draw backbone
	//ST7920_Init();
	//ST7920_Clear();

	ST7920_ClearBuffer();
	ST7920_GraphicMode(1);

	DrawRectangle(0, 0, 127, 63);
	DrawRectangle(0, 0, 127, 14);

	//draw low rectangles
	DrawRectangle(0, 48, 40, 15);
	DrawRectangle(44, 48, 40, 15);
	DrawRectangle(87, 48, 40, 15);


	// 2- make right state
	if(state%2 == 0){
		DrawFilledRectangle(0, 16, 127, 15);
	}
	else if(state%2 == 1){
		DrawFilledRectangle(0, 32, 127, 15);
	}

	ST7920_Update();


	// 3- put text
	ST7920_GraphicMode(0);
	char line1[6];
	char line2[6];
	char line3[5];


	sprintf(line1, "%s", parameter[0].code);
	sprintf(line2, "%s", parameter[1].code);

	sprintf(line3, "%lu", parameter[state%2].value_milli);

	ST7920_SendString(0,2,"SETTINGS");

	//clear lines
	ST7920_SendString(1,0, "                ");
	ST7920_SendString(2,0, "                ");

	ST7920_SendString(1,0, line1);
	ST7920_SendString(2,0, line2);
	ST7920_SendString((state%2 + 1),4, line3);


	//give possible actions
	ST7920_SendString(3,3, "ENT");
	ST7920_SendString(3,6, "ESC");
}
void drawParameterPage(){
	// 1- draw backbone
	// 2- put text


	//1-
	//ST7920_Init();
	//ST7920_Clear();
	ST7920_ClearBuffer();
	ST7920_GraphicMode(1);

	DrawFilledRectangle(0, 0, 127, 15);
	DrawRectangle(0, 0, 127, 63);


	//draw low rectangles
	if(!save){
		DrawFilledRectangle(0, 48, 40, 15);
		DrawFilledRectangle(44, 48, 40, 15);
		DrawFilledRectangle(87, 48, 40, 15);
	}
	else{
		DrawFilledRectangle(0, 48, 127, 15);
	}


	ST7920_Update();

	//2-
	ST7920_GraphicMode(0);

	char line1[6];
	char line2[5];
	char line3[4];

	sprintf(line1, "%s", currentParameter->code);
	sprintf(line2, "%lu", holdParameterValue);
	sprintf(line3, "%s", currentParameter->unit);

	//clear
	ST7920_SendString(1,0, "                ");

	ST7920_SendString(0,3, line1);
	ST7920_SendString(1,0, line2);
	ST7920_SendString(2,0, line3);

	if(!save){
		//give possible actions
		ST7920_SendString(3,3, "ENT");
		ST7920_SendString(3,6, "ESC");
	}
	else{

		ST7920_SendString(3,2, "SAVED");
		HAL_Delay(500);

	}


}

void Encoder_Init(){

	/* Init interface Encoder Begin*/
	oJog.initEncoder(ENC_A_GPIO_Port, ENC_A_Pin, ENC_B_GPIO_Port,ENC_B_Pin);
	oJog.setIncrementStep(30);
	oJog.setMaxEncoderValue(65535);
	oJog.setMinEncoderValue(0);
	oJog.setCurrentEncoderValue(0);
	//oJog.setUserForwardCallback(Jog_ForwardCallback);
	//oJog.setUserBackwardCallback(Jog_BackwardCallback);
}



/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_IWDG_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */

	// init variables



	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	{
		/* htim2 Generation Error */
		Error_Handler();
	}







	Encoder_Init();
	Paramertes_Init();
	/* this is where i load the saved data from Flash*/
	read_flash_memory(PERSISTENT_DATA_START, toFlashData, 3);
	parameter[0].value_milli = toFlashData[F_TDETECT] ;
	parameter[1].value_milli = toFlashData[F_TWAIT] ;
	/*
	if(parameter[0].value_milli  < parameter[0].min){
		parameter[0].value_milli  = parameter[0].min;
	}
	if(parameter[1].value_milli  < parameter[1].min){
		parameter[1].value_milli  = parameter[1].min;
	}*/
	StateMode_t = (Ts_StateMode)toFlashData[F_STATE] ;



	KeyPress_t = KEY_NONE;

	logoAnimation();




	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */


	while (1)
	{


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		//check input sensor
		detect();

		//examin button press
		switch (KeyPress_t) {
		case KEY_RESET:

			if(StateMode_t == STATE_ALARM){
				StateMode_t = STATE_WAIT;
				clear_screen =1;
			}
			else if(StateMode_t == STATE_WAIT){
				StateMode_t = STATE_RUN;
				clear_screen =1;
			}

			reinitVariables();
			KeyPress_t = KEY_NONE;

			break;

		case KEY_MODE:

			if(MainMode_t == MAIN_HOME){
				MainMode_t = MAIN_MENU;

				// jog paramerets for menu scrolling

				oJog.setMaxEncoderValue(255);
				oJog.setMinEncoderValue(setting_animation);
				oJog.setIncrementStep(1);

				clear_screen =1;
			}




			KeyPress_t = KEY_NONE;

			break;
		case KEY_ESC:

			if(MainMode_t == MAIN_PARAMETERS){
				MainMode_t = MAIN_MENU;
				oJog.setMaxEncoderValue(255);
				oJog.setCurrentEncoderValue(setting_animation);
				oJog.setIncrementStep(1);
				// no saving

				clear_screen =1;
			}
			else if(MainMode_t == MAIN_MENU){
				MainMode_t = MAIN_HOME;
				clear_screen =1;
			}


			KeyPress_t = KEY_NONE;
			break;
		case KEY_ENT:

			if(MainMode_t == MAIN_MENU){
				MainMode_t = MAIN_PARAMETERS;

				//set which parameter
				currentParameter = &parameter[setting_animation%2];
				oJog.setMaxEncoderValue(currentParameter->max);
				holdParameterValue = currentParameter->value_milli;
				oJog.setCurrentEncoderValue(holdParameterValue);
				oJog.setIncrementStep(30);

				clear_screen =1;
			}
			else if(MainMode_t == MAIN_PARAMETERS){
				// show it has stored

				save = 1;
				//saving function



				toFlashData[setting_animation%2 + 1] = holdParameterValue;
				currentParameter->value_milli = holdParameterValue;

				clear_screen =1;
			}


			KeyPress_t = KEY_NONE;
			break;
		default:
			break;
		}





		StateMachine();
		DisplayManagement();
		checkActivateScreen(); // activate display lighty

		update_previousTime = currentTime();

		if(save){
			/* This is where data storage is flash is done*/
			write_flash_memory(PERSISTENT_DATA_START, toFlashData, 3);
			
			save = 0;
			clear_screen =1;
		}


		//HAL_Delay(250);





	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){



	oJog.HandleGPIOEvents(GPIO_Pin);

	// other pins;
	if(GPIO_Pin == ESC_PB_Pin){
		// go out
		//if(!HAL_GPIO_ReadPin(ESC_PB_GPIO_Port, ESC_PB_Pin)){
		if(light_Display_on ){
			KeyPress_t = KEY_ESC;
		}

		light_Display = 1;
		//}
	}
	else if(GPIO_Pin == MODE_PB_Pin){
		//enter menu

		//if(!HAL_GPIO_ReadPin(MODE_PB_GPIO_Port, MODE_PB_Pin)){

		if(light_Display_on ){
			KeyPress_t = KEY_MODE;
		}

		light_Display = 1;
		//}
	}
	else if(GPIO_Pin == RST_PB_Pin){
		// reset states
		//if(!HAL_GPIO_ReadPin(RST_PB_GPIO_Port, RST_PB_Pin)){
		KeyPress_t = KEY_RESET;
		light_Display = 1;
		//}
	}
	else if(GPIO_Pin == ENC_ENT_Pin){
		// validate value
		//if(!HAL_GPIO_ReadPin(ENC_ENT_GPIO_Port, ENC_ENT_Pin)){

		if(light_Display_on ){
			KeyPress_t = KEY_ENT;
		}
		light_Display = 1;
		//}
	}

	if(GPIO_Pin == INPUT_Pin){
		/*if(!HAL_GPIO_ReadPin(ENC_ENT_GPIO_Port, ENC_ENT_Pin)){

		}
		else{

		}*/
		//just a change in state is sufficient to say everything is ok
		detected = 1;
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	if(htim->Instance == TIM3){
		/*
		 * TIMER3 counts at 2000Hz/500 us per count
		 *
		 */
		holdCurrenTimeCount += 1;

		if((currentTime() - state_Machine_previousTime) >= 1000 && remainingTimeValue >0){
			remainingTimeValue -=1;
			state_Machine_previousTime = currentTime();
		}

		HAL_IWDG_Refresh(&hiwdg);// watchdog
	}



}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
