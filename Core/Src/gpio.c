/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "SystemStates.h"
#include "Functions.h"
#include "tim.h"
#include "string.h"
#include "usart.h"
#include "TrackPad.h"
#include "iqs7211a_addresses.h"
#include "IQS7211A_init_AZP1189A3_v0.1.h"

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */


volatile uint8_t White_LED=0;

volatile uint32_t Last_Button_Press=0; //Stores the time the last button was registered

volatile uint8_t Prev_State_Middle=1; //Stores the previous state of the button before the interrupt was triggered
volatile uint8_t Prev_State_Right=1; //Stores the previous state of the button before the interrupt was triggered
volatile uint8_t Prev_State_Left=1; //Stores the previous state of the button before the interrupt was triggered

volatile uint8_t Button_State; //Stores the current state of the button as updated in the interrupt
volatile uint32_t Current_Time; //Stores the current time that the interrupt has occurred

volatile SystemState current_state=Flashlight; // Sets the initial state of the system to Flashlight
volatile EmergencyMode current_emergency=Strobe;//Sets the current emergency mode to Strobe

volatile uint16_t Strobe_Time=512;

volatile uint8_t Strobe_Flag=0;

volatile uint16_t Strobe_Count=0;

volatile uint8_t Morse_Index=0;

extern char CustomMessage[15];//Max possible characters in the custom message
extern char SOS_Morse[9];
extern uint8_t Morse_Length,Custom_Length;
extern uint16_t Red_Intensity,Green_Intensity,Blue_Intensity;
extern uint8_t TrackPadReady;
extern uint32_t TrackPadPauseTime;
extern uint8_t CustomSet;

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LED_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_5_Pin|LED_3_Pin|LED_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCLR_GPIO_Port, MCLR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = LD2_Pin|LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Ready_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Ready_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = LED_5_Pin|LED_3_Pin|LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = Middle_Pin|Left_Pin|Right_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MCLR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MCLR_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	Current_Time=HAL_GetTick();

//&&((HAL_GetTick()-TrackPadPauseTime)>50)
	if ((GPIO_Pin==Ready_Pin)&&(TrackPadReady)){//Only starts looking for gestures once the track pad is setup

		TrackPadInput();
		SystemRefresh();
	}



	//Button Debouncing for Middle Button-GPIO_13
	if ((GPIO_Pin == GPIO_PIN_13)&&(Current_Time-Last_Button_Press>20)){
		Button_State= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);
		if ((Prev_State_Middle==1)&&(Button_State==0)){


			White_LED= !White_LED;
			Strobe_Flag=1;
			SystemRefresh();//Refreshes the system after any button click has occurred
			Last_Button_Press=Current_Time;
			Prev_State_Middle=Button_State;
		}
		if ((Prev_State_Middle==0)&&(Button_State==1)){
			Last_Button_Press=Current_Time;
			Prev_State_Middle=Button_State;
		}
	}


	//Button Debouncing for Left Button-GPIO_14
	if ((GPIO_Pin == GPIO_PIN_14)&&(Current_Time-Last_Button_Press>20)){
		Button_State= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
		if ((Prev_State_Left==1)&&(Button_State==0)){

			if (current_state==Mood){
				current_state=Flashlight;
				White_LED=0; //Sets the White LED to OFF when changing to flashlight mode
			} else current_state++; //Checks if the state is in the final state and then wraps round to the beginning
			if (current_state==Emergency){//Sets the emergency mode to Strobe when changing to Emergency state
				current_emergency=Strobe;
				Strobe_Time=512;
				Strobe_Count=0;
				White_LED=0; //Sets the White LED to OFF when changing to emergency mode
			} else if (current_state ==Mood){
				Red_Intensity=128;
				Green_Intensity=128;
				Blue_Intensity=128;
				White_LED=0; //Sets the White LED to OFF when changing to Mood mode
			}
			SystemRefresh();//Refreshes the system after any button click has occurred
			Last_Button_Press=Current_Time;
			Prev_State_Left=Button_State;
		}
		if ((Prev_State_Left==0)&&(Button_State==1)){
			Last_Button_Press=Current_Time;
			Prev_State_Left=Button_State;
		}
	}
	//Button Debouncing for Right Button-GPIO_15
	if ((GPIO_Pin == GPIO_PIN_15)&&(Current_Time-Last_Button_Press>20)){
		Button_State= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
		if ((Prev_State_Right==1)&&(Button_State==0)){

			if(current_state==Emergency){

				if ((current_emergency==Strobe)){
					Morse_Index=0;//Resets the morse index on changing state
					Strobe_Flag=0;//Starts in off state
					Morse_Length=12;
					//White_LED=1;
				}else if (current_emergency==SOS){

					//strcpy(CustomMessage,SOS_Morse);
					//Morse_Length=9;
					//White_LED=1;
					if (CustomSet){
						Morse_Length=Custom_Length;
					}
					Morse_Index=0;//Resets the morse index on changing state
					Strobe_Flag=0;//Starts in off state
				}
				if (current_emergency==custom_message){
					current_emergency=Strobe;
					Strobe_Time=512;
				}else current_emergency++;
			}

			SystemRefresh();//Refreshes the system after any button click has occurred
			Last_Button_Press=Current_Time;
			Prev_State_Right=Button_State;
		}
		if ((Prev_State_Right==0)&&(Button_State==1)){
			Last_Button_Press=Current_Time;
			Prev_State_Right=Button_State;
		}
	}


}
/* USER CODE END 2 */
