/*
 * Functions.c
 *
 *  Created on: 18 Mar 2023
 *      Author: Aidan
 */

#include "Functions.h"
#include "SystemStates.h"
#include "gpio.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "String.h"
#include "stdlib.h"
#include "adc.h"
#include "Morse.h"
#include "stdio.h"
#include "i2c.h"
#include "iqs7211a_addresses.h"
#include "IQS7211A_init_AZP1189A3_v0.1.h"
#include "TrackPad.h"

///////////////////System State Variables///////////////////////

extern SystemState current_state;
extern EmergencyMode current_emergency;

///////////////////System State Variables///////////////////////

////////////////Global Variable Instantiation//////////////////

extern uint16_t Light_Intensity, Red_Intensity, Green_Intensity, Blue_Intensity;
extern uint16_t Strobe_Time;
extern uint8_t White_LED;
extern uint8_t Strobe_Flag; //The flag that controls the strobing of the White LED

extern uint16_t Strobe_Count;
extern uint8_t Morse_Index;

extern uint8_t TrackPadReady;

////////////////Global Variable Instantiation//////////////////



////////////////Slider Variables//////////////////
uint32_t slider_value = 1, prev_slider_value = 1;
uint8_t Buffer = 200;
uint16_t Light_Intensity = 0;
char adc_value[16];

////////////////Slider Variables//////////////////

/////////////////////////////////////Local Function Variables////////////////////////////////////////////

char Mode[2], State[4], Param1[4], Param2[4] = "000\0",MorseParam2[4]="SOS\0"; //Instantiating the char arrays for the UART decoding

uint16_t State_int = 0, Param1_int = 0, Param2_int = 0;

uint8_t TrackPadReady=0;
uint8_t HoldFlag=0;
uint8_t slideDelay=0;
uint16_t prevabsX=0;
uint32_t slideTime=0;

/////////////////////////UART Receiving Variables////////////////////////////////////
extern char RxCharacter; //The char for receiving the UART char by char
char Command[25]; //The array to receive the UART command
uint8_t UART_Index = 0; //The index used to build the command
/////////////////////////UART Receiving Variables////////////////////////////////////

char DisplayState[19]; //Array used to send back the current system state
char CustomMessage[25] = "...*---*... "; //Max possible characters in the custom message
char SOS_Morse[12] = "...*---*... ";

uint8_t Morse_Length = 12; //Standard length of the SOS_Morse message
uint8_t Custom_Length=12;//Standard size of the custom_length
uint8_t CustomSet=0;
uint8_t Time_Units = 1;

char Test[4]; //Testing char array

//////////////////////////////////////Local Function Variables////////////////////////////////////////////

void TrackPadInput(){

		if (HAL_GetTick()-slideTime>250){//The user has stopped sliding for over 250ms

			slideDelay=0;//Reset the delay needed to start a slide motion
		}



		Gesture prevGesture=NoGesture;

		uint8_t gestureData[10];

		uint16_t relX=0;
		uint16_t relY=0;

		uint16_t absX=0;
		int16_t  deltaX=0;

		int16_t absY=0;//Cannot be unsigned because of the inversion steps taken
		char relXStr[25],relYStr[25],absXStr[25],absYStr[25];



		readByte(IQS7211A_MM_GESTURES,10,gestureData);


			relX= (uint16_t)gestureData[2];
			relX|= (uint16_t)(gestureData[3]<<8);
			relY= (uint16_t)gestureData[4];
			relY|= (uint16_t)(gestureData[5]<<8);

			absX= (uint16_t)gestureData[6];
			absX|= (uint16_t)(gestureData[7]<<8);
			absY= (int16_t)gestureData[8];
			absY|= (int16_t)(gestureData[9]<<8);
			///////////Invert Y Values////////////////
			absY-=766;
			absY=absY*-0.7014;

			if(absY>510){absY=512;}
			if(absY<5){absY=0;}
			///////////Invert Y Values////////////////

			/*sprintf(relXStr,"Relative X: %05d\n",relX);
			sprintf(relYStr,"Relative Y: %05d\n",relY);
			sprintf(absXStr,"Absolute X: %05d\n",absX);
			sprintf(absYStr,"Absolute Y: %05d\n",absY);*/

			//HAL_UART_Transmit(&huart2,(uint16_t*)relXStr,strlen(relXStr),50);

			switch (gestureData[0]){

			case Tap:
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
				if (current_state==Mood){

					if (absX>1172){
						Blue_Intensity=absY;
					}else if (absX>586){
						Green_Intensity=absY;
					}else{
						Red_Intensity=absY;
					}
				}
				slideDelay=0;
				//HAL_UART_Transmit(&huart2,(uint8_t*)"Tap\n",strlen("Tap\n"),50);
				prevGesture=Tap;
				break;
			case Hold:
				HoldFlag=1;
				slideDelay=0;
				prevGesture=Hold;
				break;
			case SwipeLeft:
				//HAL_UART_Transmit(&huart2,(uint8_t*)"SwipeLeft\n",10,50);
				slideDelay=0;
				prevGesture=SwipeLeft;
				break;
			case SwipeRight:
				//HAL_UART_Transmit(&huart2,(uint8_t*)"SwipeRight\n",11,50);
				slideDelay=0;
				prevGesture=SwipeRight;
				break;
			case SwipeUp:
				//HAL_UART_Transmit(&huart2,(uint8_t*)"SwipeUp\n",strlen("SwipeUp\n"),50);
				slideDelay=0;
				prevGesture=SwipeUp;
				break;
			case SwipeDown:
				//HAL_UART_Transmit(&huart2,(uint8_t*)"SwipeDown\n",strlen("SwipeDown\n"),50);
				slideDelay=0;
				prevGesture=SwipeDown;
				break;
			case NoGesture:
				if (HoldFlag){
					White_LED= !White_LED;
					HoldFlag=0;
					slideDelay=0;
					//HAL_UART_Transmit(&huart2,(uint8_t*)"Toggled\n",strlen("Toggled\n"),50);
				}else if (slideDelay>=20){

					if ((absX<1792)&&(prevabsX<1792)){

						deltaX=abs((absX-prevabsX))*0.388;

						if(absX>prevabsX){
							Light_Intensity+=deltaX;
						}else {
							if (deltaX>=Light_Intensity){
								Light_Intensity=1;
							}else{Light_Intensity-=deltaX;}
						}

						if (Light_Intensity>512){Light_Intensity=512;}

						slideTime=HAL_GetTick();//Updates the slide time to the most recent tick to show that the user is currently sliding
						//sprintf(relXStr,"Intensity: %05d\n",Light_Intensity);
						//HAL_UART_Transmit(&huart2,(uint16_t*)relXStr,strlen(relXStr),50);
					}

				} else if (prevGesture==NoGesture){
					slideDelay++;
					slideTime=HAL_GetTick();//Updates the slide time to the most recent tick to show that the user is currently sliding
				}
				prevGesture=NoGesture;

				break;
			default:
				break;

			}
			prevabsX=absX;

}


void TrackPadSetup(){


	//Hardware Reset
	HAL_GPIO_WritePin(GPIOC, MCLR_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, MCLR_Pin,1);


	uint8_t setupBuffer[2];

	//Read Info Flags
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	readByte(IQS7211A_MM_INFOFLAGS,2,setupBuffer);
	HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
	setupBuffer[0]&=0x80;//Toggles all bits low to check if it has already been reset

	if (setupBuffer[0]==0){//Checks if the track pad has not been reset and then resets it

		while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
		readByte(IQS7211A_MM_SYSTEM_CONTROL,2,setupBuffer);
		HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
		setupBuffer[1]|=0x02;
		while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
		writeByte(IQS7211A_MM_SYSTEM_CONTROL,2,setupBuffer);
		HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

	}

	//Read 0x50, set bit 7 and write back to 0x50
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	readByte(IQS7211A_MM_SYSTEM_CONTROL,2,setupBuffer);
	HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
	setupBuffer[0]|=0x80;//Reset the acknowledge bit
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	writeByte(IQS7211A_MM_SYSTEM_CONTROL,2,setupBuffer);
	HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

	//Read Product Number
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	getProductNum();

	//Write setting file
	writeMM();

	//TP Re-ATI
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    readByte(IQS7211A_MM_SYSTEM_CONTROL,1,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
    setupBuffer[0]|=0x20;
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    writeByte(IQS7211A_MM_SYSTEM_CONTROL,1,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

    //Enable Gesture Events
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    readByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
    setupBuffer[1]|=0x02; //Enables gesture events
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    writeByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

    //Enable Touch Pad Events
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    readByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
    setupBuffer[1]|=0x04;
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    writeByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

    //Set track pad to Event Mode
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    readByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data
    setupBuffer[1]|=0x01;
	while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
    writeByte(IQS7211A_MM_CONFIG_SETTINGS,2,setupBuffer);
    HAL_Delay(1);//This is just here to prevent another read/write byte starting immediately as this one ends corrupting data

	//Set TrackPadSetup=1
    TrackPadReady=1;

   // HAL_UART_Transmit(&huart2,(uint8_t*)"TrackPad Ready\n",strlen("TrackPad Ready\n"),100);

}

void test(){


	uint8_t testread [8];

	readByte(IQS7211A_MM_TP_SETTINGS_0, 8, testread);


	//HAL_UART_Transmit(&huart2,(uint8_t*)testread,8,200);

}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (RxCharacter == '\n') { //The end of the command has been received
		UART_Command(UART_Index); //Calls the UART command function
		UART_Index = 0;

	} else {
		Command[UART_Index++] = RxCharacter;
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t*) &RxCharacter, 1);

}

void UART_Command(uint8_t CommandLength) {

	if ((Command[0] == '#') && (Command[CommandLength - 1] == '$')) { //Checks that a valid command has been received

		if (CommandLength == 6) { //Checks to see if the command received is a request for system state updates

			memcpy(Mode, &Command[2], 2);
			memset(DisplayState, '\0', sizeof(DisplayState)); //Resets the display state array
			DisplayState[0] = '#';
			DisplayState[1] = ':';
			strcat(DisplayState, Mode); //#:MF
			strcat(DisplayState, ":"); //#:MF:

			switch (Mode[1]) {

			case 'F':
				if (White_LED == 1) {
					sprintf(State, "%03d", Light_Intensity);
				} else {
					strcpy(State, "000");
				}

				strcpy(Param1, "000");
				strcpy(Param2, "000");
				break;

			case 'E':
				sprintf(State, "%03d", Light_Intensity);
				sprintf(Param1, "%03d", Strobe_Time);
				if (Param1_int != 0) {
					strcpy(Param2, "000");
				}
				if (current_emergency==Strobe){
					strcpy(Param2, "000");
				}else if (current_emergency==SOS) {
					strcpy(Param1, "000");//TODO check that this must be zero when in SOS mode and UART requests state
					strcpy(Param2, "SOS");
				}else if (current_emergency==custom_message){
					strcpy(Param1, "000");
					strcpy(Param2, MorseParam2);
				}

				break;

			case 'M':
				sprintf(State, "%03d", Red_Intensity);
				if (White_LED == 0) {
								strcpy(Param1, "000");
								strcpy(Param2, "000");
				}else{
					sprintf(Param1, "%03d", Green_Intensity);
					sprintf(Param2, "%03d", Blue_Intensity);
				}
				break;

			}

			if (White_LED == 0) {
				strcpy(State, "000");
			}
			strncat(DisplayState, State, 3); //#:MF:512
			strcat(DisplayState, ":");
			strncat(DisplayState, Param1, 3); //#:MF:512:000
			strcat(DisplayState, ":");
			strncat(DisplayState, Param2, 3); //#:MF:512:000:000
			strcat(DisplayState, ":$\n");
			HAL_UART_Transmit(&huart2, (uint8_t*) &DisplayState,
					sizeof(DisplayState), 100);

		} else if (CommandLength == 18) { //Checks to see if the command received is a request to change the system state

			//Splitting the Command up into its separate parameters
			memcpy(Mode, &Command[2], 2);
			memcpy(State, &Command[5], 3);
			memcpy(Param1, &Command[9], 3);
			memcpy(Param2, &Command[13], 3);

			State_int = atoi(State); //Converts the string for the State value to an integer value
			Param1_int = atoi(Param1); //Converts the string for Param1 to an integer value
			Param2_int = atoi(Param2); //Converts the string for Param2 to an integer value

			switch (Mode[1]) {

			case 'F':
				//Param1 and Param2 must always be 000 here - CHECK?!?!?!?!
				current_state = Flashlight;
				White_LED = 1;
				Light_Intensity = State_int;
				SystemRefresh();
				break;

			case 'E':
				current_state = Emergency;
				Light_Intensity = State_int;
				if (Param1_int != 0) {
					White_LED = 1;
					Strobe_Time = Param1_int;
					current_emergency = Strobe;
					SystemRefresh();
					break;
				} else {
					Strobe_Time = 0;

					if (!strcmp(Param2, "000")) { //Checks if Param2 is equal to 000
						current_emergency = SOS;
						White_LED = 1;
						Strobe_Flag = 0;
						Morse_Index = 0;
						Morse_Length = 12;

						break;
					} else {
						strcpy(MorseParam2,Param2);
						current_emergency = custom_message;
						White_LED = 1;
						Strobe_Flag = 0;
						Morse_Index = 0; //Resets the index to start at the beginning of the array
						Custom_Length = 0; //Resets the Morse length to 0 for the custom message
						CustomSet=1;//Sets the custom message flag to set
						memset(CustomMessage, '\0', sizeof(CustomMessage)); //Resets the custom message to NULL
						DecodeMorse();
						break;
					}

				}

				break;

			case 'M':
				White_LED=1;
				current_state = Mood;
				Red_Intensity = State_int;
				Green_Intensity = Param1_int;
				Blue_Intensity = Param2_int;
				SystemRefresh();
				break;
			}
			SystemRefresh();
		}

	}

}

void SystemRefresh() {

	debugLEDs();

	if (White_LED) {

		if (current_state==Mood){
			TIM2->CCR4 = Blue_Intensity; //BLUE
			TIM3->CCR3 = Red_Intensity; //RED
			TIM4->CCR2 = Green_Intensity; //GREEN
			TIM1->CCR1 = 0; //Sets the PWM Signal Value
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //Sets DAC Value to zero cause the LED should be off
		}else{
			TIM1->CCR1 = Light_Intensity; //Sets the PWM Signal Value
			TIM2->CCR4 = 0; //BLUE
			TIM3->CCR3 = 0; //RED
			TIM4->CCR2 = 0; //GREEN



					if (Light_Intensity != 0) { //Better way of doing this by toggle White_LED instead but meh
						HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,
								(Light_Intensity * 8) - 1); //Sets DAC Value
					} else {
						HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //Sets DAC Value
					}

					if ((!Strobe_Flag) && (current_state == Emergency)) { //Every time the strobe flag falls the pins are overwritten to zero
						__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); //Sets the PWM Signal Value
						HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //Sets DAC Value to zero cause the LED should be off
						HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
					}
		}


	} else {
		//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);//Sets the PWM Signal Value
		TIM1->CCR1 = 0; //Sets the PWM Signal Value
		TIM2->CCR4 = 0; //BLUE
		TIM3->CCR3 = 0; //RED
		TIM4->CCR2 = 0; //GREEN
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //Sets DAC Value to zero cause the LED should be off

	}

}

void DecodeMorse() {

	for (int i = 0; i < 3; i++) {

		MorseIndex(MorseParam2[i]);
		if (i < 2) {
			strcat(CustomMessage, "*");
			Custom_Length++;
		}
	}
	strcat(CustomMessage, " ");
	Custom_Length++;
	Morse_Length=Custom_Length;


}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if ((htim == &htim16) && (current_state == Emergency)
			&& (current_emergency == Strobe)) {

		Strobe_Count++;
		if (Strobe_Count >= Strobe_Time) {
			Strobe_Count = 0;
			Strobe_Flag = !Strobe_Flag; //Toggles the white LED output.
			SystemRefresh(); //Refreshes the system
		}

	}

	if ((htim == &htim17) && (current_state == Emergency)) {

		if (current_emergency == SOS) {

			ExecuteMorse(SOS_Morse); //Executes the morse code using the SOS morse message

		} else if (current_emergency == custom_message) {

			ExecuteMorse(CustomMessage); ////Executes the morse code using the Custom Message Morse

		}

	}

}

void ExecuteMorse(char *MorseCode) {

	if ((Morse_Index >= Morse_Length)) {

		Morse_Index = 0;
		Time_Units = 1;
	} else {
		char c = MorseCode[Morse_Index]; //...---...
		if (c == '.') {
			if (Strobe_Flag == 1) {
				Morse_Index++;
				Strobe_Flag = !Strobe_Flag;
			} else {
				Strobe_Flag = !Strobe_Flag;
			}
		} else if (c == '-') {
			if (Strobe_Flag == 1) {
				if (Time_Units == 3) {
					Morse_Index++;
					Time_Units = 1;
					Strobe_Flag = !Strobe_Flag;
				} else {
					Time_Units++;
				}
			} else {
				Strobe_Flag = !Strobe_Flag;
			}
		} else if (c == '*') {

			if (Time_Units == 2) {
				Morse_Index++;
				Time_Units = 1;
			} else {
				Time_Units++;
			}
		} else if (c == ' ') {
			if (Time_Units == 5) {
				Morse_Index++;
				Time_Units = 1;
			} else {
				Time_Units++;
			}

		}
		SystemRefresh(); //Refreshes the system
	}

}

void Setup() {


	HAL_TIM_Base_Start_IT(&htim16); //Starts the interrupt for timer 16
	HAL_TIM_Base_Start_IT(&htim17); //Starts the interrupt for timer 17

	HAL_UART_Receive_IT(&huart2, (uint8_t*) &RxCharacter, 1); //Setups up the UART to receive 1 character over DMA and then call RxCplt

	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);//RED
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);//GREEN
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);//BLUE

	//Sending student number over UART on startup
	HAL_Delay(200);
    HAL_UART_Transmit(&huart2, (uint8_t*) "#:23993103:$\n",
			strlen("#:23993103:$\n"), 300);

}

void Slider_Update() {

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 300);

	slider_value = HAL_ADC_GetValue(&hadc1);

	HAL_Delay(2);
	if (abs(slider_value - prev_slider_value) > Buffer) {

		prev_slider_value = slider_value;
		Light_Intensity = slider_value;

		if (slider_value>=23){
			Light_Intensity = (slider_value - 23) * 1.0775 / 8; //Added buffer to shift center point and then divided it out the top end
		}else {Light_Intensity=1;}

		if (Light_Intensity <= 23) {
			Light_Intensity = 1;
		} //Tapers lower value to 0 always
		if (Light_Intensity >= 512) {
			Light_Intensity = 512;
		} //Tapers upper value to 512 always

	//sprintf(adc_value, "%03d\n", Light_Intensity);
	//HAL_UART_Transmit(&huart2, (uint8_t*) adc_value, 4, 300);
		SystemRefresh(); //Refreshes the system when the slider changes

	}

}


void debugLEDs(){





	switch (current_state){

	case Flashlight:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1); //SET LED 2
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); //LED 3
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); //LED 4
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0); //LED 5
		break;
	case Emergency:

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0); //LED 2
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1); //SET LED 3
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); //LED 4

		if (current_emergency==SOS||current_emergency==custom_message){
			//TODO Set the last LED on as well and include switching it off in all other states
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1); //LED 5
		}else if(current_emergency==Strobe){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0); //LED 5
		}
	break;

	case Mood:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0); //LED 2
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); //LED 3
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1); //SET LED 4
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0); //LED 5
	break;
	}
}


