/*
 * TrackPad.c
 *
 *  Created on: Apr 26, 2023
 *      Author: Aidan
 */





#include "i2c.h"
#include "iqs7211a_addresses.h"
#include "IQS7211A_init_AZP1189A3_v0.1.h"
#include "usart.h"
#include "SystemStates.h"
#include "string.h"
#include "stdlib.h"

#define DevAddress 0x56





void writeByte(uint8_t MemAddress,uint8_t numBytes, uint8_t data[]){

	HAL_I2C_Mem_Write (&hi2c1,DevAddress<<1,MemAddress,1,(uint8_t*)data,numBytes,1000);


}


void readByte(uint8_t MemAddress,uint8_t numBytes, uint8_t data[]){

	if(!HAL_GPIO_ReadPin(GPIOC,Ready_Pin)){
		HAL_I2C_Mem_Read (&hi2c1,DevAddress<<1,MemAddress,1,(uint8_t*)data,numBytes,1000);
	}




}



void getProductNum(){

	char productlow[5];

	uint8_t transferBytes[2];

	uint8_t prodNumLow =0;
	uint8_t prodNumHigh =0;
	uint16_t prodNumReturn =0;


	readByte(IQS7211A_MM_PROD_NUM,2,transferBytes);


	prodNumLow = transferBytes[0];
	prodNumHigh = transferBytes[1];
	prodNumReturn = (uint16_t) (prodNumLow);
	prodNumReturn |= (uint16_t) (prodNumHigh<<8);

	sprintf(productlow, "%03d\n", prodNumReturn);
	if (prodNumLow>0){
		//HAL_UART_Transmit(&huart2,(uint8_t*)productlow,4,50);
	}

}


void writeMM(){
	uint8_t transferBytes[30];	// Temporary array which holds the bytes to be transferred.

	  /* Change the ATI Settings */
	  /* Memory Map Position 0x30 - 0x3D */
	  transferBytes[0] = TP_ATI_MULTIPLIERS_DIVIDERS_0;
	  transferBytes[1] = TP_ATI_MULTIPLIERS_DIVIDERS_1;
	  transferBytes[2] = TP_COMPENSATION_DIV_0;
	  transferBytes[3] = TP_COMPENSATION_DIV_1;
	  transferBytes[4] = TP_ATI_TARGET_0;
	  transferBytes[5] = TP_ATI_TARGET_1;
	  transferBytes[6] = TP_REF_DRIFT_LIMIT_0;
	  transferBytes[7] = TP_REF_DRIFT_LIMIT_1;
	  transferBytes[8] = TP_MIN_COUNT_REATI_0;
	  transferBytes[9] = TP_MIN_COUNT_REATI_1;
	  transferBytes[10] = REATI_RETRY_TIME_0;
	  transferBytes[11] = REATI_RETRY_TIME_1;
	  transferBytes[12] = ALP_ATI_MULTIPLIERS_DIVIDERS_0;
	  transferBytes[13] = ALP_ATI_MULTIPLIERS_DIVIDERS_1;
	  transferBytes[14] = ALP_COMPENSATION_DIV_0;
	  transferBytes[15] = ALP_COMPENSATION_DIV_1;
	  transferBytes[16] = ALP_ATI_TARGET_0;
	  transferBytes[17] = ALP_ATI_TARGET_1;
	  transferBytes[18] = ALP_LTA_DRIFT_LIMIT_0;
	  transferBytes[19] = ALP_LTA_DRIFT_LIMIT_1;

	  /* Change the ALP ATI Compensation */
	  /* Memory Map Position 0x3A - 0x3D */
	  transferBytes[20] = ALP_COMPENSATION_A_0;
	  transferBytes[21] = ALP_COMPENSATION_A_1;
	  transferBytes[22] = ALP_COMPENSATION_B_0;
	  transferBytes[23] = ALP_COMPENSATION_B_1;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_TP_ATI_MIR, 24, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the Report Rates and Timing */
	  /* Memory Map Position 0x40 - 0x4A */
	  transferBytes[0] = ACTIVE_MODE_REPORT_RATE_0;
	  transferBytes[1] = ACTIVE_MODE_REPORT_RATE_1;
	  transferBytes[2] = IDLE_TOUCH_MODE_REPORT_RATE_0;
	  transferBytes[3] = IDLE_TOUCH_MODE_REPORT_RATE_1;
	  transferBytes[4] = IDLE_MODE_REPORT_RATE_0;
	  transferBytes[5] = IDLE_MODE_REPORT_RATE_1;
	  transferBytes[6] = LP1_MODE_REPORT_RATE_0;
	  transferBytes[7] = LP1_MODE_REPORT_RATE_1;
	  transferBytes[8] = LP2_MODE_REPORT_RATE_0;
	  transferBytes[9] = LP2_MODE_REPORT_RATE_1;
	  transferBytes[10] = ACTIVE_MODE_TIMEOUT_0;
	  transferBytes[11] = ACTIVE_MODE_TIMEOUT_1;
	  transferBytes[12] = IDLE_TOUCH_MODE_TIMEOUT_0;
	  transferBytes[13] = IDLE_TOUCH_MODE_TIMEOUT_1;
	  transferBytes[14] = IDLE_MODE_TIMEOUT_0;
	  transferBytes[15] = IDLE_MODE_TIMEOUT_1;
	  transferBytes[16] = LP1_MODE_TIMEOUT_0;
	  transferBytes[17] = LP1_MODE_TIMEOUT_1;
	  transferBytes[18] = REF_UPDATE_TIME_0;
	  transferBytes[19] = REF_UPDATE_TIME_1;
	  transferBytes[20] = I2C_TIMEOUT_0;
	  transferBytes[21] = I2C_TIMEOUT_1;
		while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_ACTIVE_MODE_RR, 22, transferBytes);


	  /* Change the System Settings */
	  /* Memory Map Position 0x50 - 0x5B */
	  transferBytes[0] = SYSTEM_CONTROL_0;
	  transferBytes[1] = SYSTEM_CONTROL_1;
	  transferBytes[2] = CONFIG_SETTINGS0;
	  transferBytes[3] = CONFIG_SETTINGS1;
	  transferBytes[4] = OTHER_SETTINGS_0;
	  transferBytes[5] = OTHER_SETTINGS_1;
	  transferBytes[6] = TRACKPAD_TOUCH_SET_THRESHOLD;
	  transferBytes[7] = TRACKPAD_TOUCH_CLEAR_THRESHOLD;
	  transferBytes[8] = ALP_THRESHOLD_0;
	  transferBytes[9] = ALP_THRESHOLD_1;
	  transferBytes[10] = OPEN_0_0;
	  transferBytes[11] = OPEN_0_1;
	  transferBytes[12] = ALP_SET_DEBOUNCE;
	  transferBytes[13] = ALP_CLEAR_DEBOUNCE;
	  transferBytes[14] = OPEN_1_0;
	  transferBytes[15] = OPEN_1_1;
	  transferBytes[16] = TP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;
	  transferBytes[17] = TP_CONVERSION_FREQUENCY_FRACTION_VALUE;
	  transferBytes[18] = ALP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;
	  transferBytes[19] = ALP_CONVERSION_FREQUENCY_FRACTION_VALUE;
	  transferBytes[20] = TRACKPAD_HARDWARE_SETTINGS_0;
	  transferBytes[21] = TRACKPAD_HARDWARE_SETTINGS_1;
	  transferBytes[22] = ALP_HARDWARE_SETTINGS_0;
	  transferBytes[23] = ALP_HARDWARE_SETTINGS_1;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_SYSTEM_CONTROL, 24, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the Trackpad Settings */
	  /* Memory Map Position 0x60 - 0x69 */
	  transferBytes[0] = TRACKPAD_SETTINGS_0_0;
	  transferBytes[1] = TRACKPAD_SETTINGS_0_1;
	  transferBytes[2] = TRACKPAD_SETTINGS_1_0;
	  transferBytes[3] = TRACKPAD_SETTINGS_1_1;
	  transferBytes[4] = X_RESOLUTION_0;
	  transferBytes[5] = X_RESOLUTION_1;
	  transferBytes[6] = Y_RESOLUTION_0;
	  transferBytes[7] = Y_RESOLUTION_1;
	  transferBytes[8] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_0;
	  transferBytes[9] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_1;
	  transferBytes[10] = XY_DYNAMIC_FILTER_TOP_SPEED_0;
	  transferBytes[11] = XY_DYNAMIC_FILTER_TOP_SPEED_1;
	  transferBytes[12] = XY_DYNAMIC_FILTER_BOTTOM_BETA;
	  transferBytes[13] = XY_DYNAMIC_FILTER_STATIC_FILTER_BETA;
	  transferBytes[14] = STATIONARY_TOUCH_MOV_THRESHOLD;
	  transferBytes[15] = FINGER_SPLIT_FACTOR;
	  transferBytes[16] = X_TRIM_VALUE_0;
	  transferBytes[17] = X_TRIM_VALUE_1;
	  transferBytes[18] = Y_TRIM_VALUE_0;
	  transferBytes[19] = Y_TRIM_VALUE_1;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_TP_SETTINGS_0, 20, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the ALP Settings */
	  /* Memory Map Position 0x70 - 0x74 */
	  transferBytes[0] = ALP_COUNT_FILTER_BETA_0;
	  transferBytes[1] = OPEN_0;
	  transferBytes[2] = ALP_LTA_BETA_LP1;
	  transferBytes[3] = ALP_LTA_BETA_LP2;
	  transferBytes[4] = ALP_SETUP_0;
	  transferBytes[5] = ALP_SETUP_1;
	  transferBytes[6] = ALP_TX_ENABLE_0;
	  transferBytes[7] = ALP_TX_ENABLE_1;

	  /* Change the Settings Version Numbers */
	  /* Memory Map Position 0x74 - 0x75 */
	  transferBytes[8] = MINOR_VERSION;
	  transferBytes[9] = MAJOR_VERSION;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_ALP_COUNT_FILTER_BETA, 10, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the Gesture Settings */
	  /* Memory Map Position 0x80 - 0x8F */
	  transferBytes[0] = GESTURE_ENABLE_0;
	  transferBytes[1] = GESTURE_ENABLE_1;
	  transferBytes[2] = TAP_TIME_0;
	  transferBytes[3] = TAP_TIME_1;
	  transferBytes[4] = TAP_DISTANCE_0;
	  transferBytes[5] = TAP_DISTANCE_1;
	  transferBytes[6] = HOLD_TIME_0;
	  transferBytes[7] = HOLD_TIME_1;
	  transferBytes[8] = SWIPE_TIME_0;
	  transferBytes[9] = SWIPE_TIME_1;
	  transferBytes[10] = SWIPE_X_DISTANCE_0;
	  transferBytes[11] = SWIPE_X_DISTANCE_1;
	  transferBytes[12] = SWIPE_Y_DISTANCE_0;
	  transferBytes[13] = SWIPE_Y_DISTANCE_1;
	  transferBytes[14] = SWIPE_ANGLE_0;
	  transferBytes[15] = GESTURE_OPEN_0;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_GESTURE_ENABLE, 16, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the RxTx Mapping */
	  /* Memory Map Position 0x90 - 0x9C */
	  transferBytes[0] = RX_TX_MAP_0;
	  transferBytes[1] = RX_TX_MAP_1;
	  transferBytes[2] = RX_TX_MAP_2;
	  transferBytes[3] = RX_TX_MAP_3;
	  transferBytes[4] = RX_TX_MAP_4;
	  transferBytes[5] = RX_TX_MAP_5;
	  transferBytes[6] = RX_TX_MAP_6;
	  transferBytes[7] = RX_TX_MAP_7;
	  transferBytes[8] = RX_TX_MAP_8;
	  transferBytes[9] = RX_TX_MAP_9;
	  transferBytes[10] = RX_TX_MAP_10;
	  transferBytes[11] = RX_TX_MAP_11;
	  transferBytes[12] = RX_TX_MAP_12;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_RXTX_MAPPING_1_0, 13, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the Allocation of channels into cycles 0-9 */
	  /* Memory Map Position 0xA0 - 0xBD */
	  transferBytes[0] = PLACEHOLDER_0;
	  transferBytes[1] = CH_1_CYCLE_0;
	  transferBytes[2] = CH_2_CYCLE_0;
	  transferBytes[3] = PLACEHOLDER_1;
	  transferBytes[4] = CH_1_CYCLE_1;
	  transferBytes[5] = CH_2_CYCLE_1;
	  transferBytes[6] = PLACEHOLDER_2;
	  transferBytes[7] = CH_1_CYCLE_2;
	  transferBytes[8] = CH_2_CYCLE_2;
	  transferBytes[9] = PLACEHOLDER_3;
	  transferBytes[10] = CH_1_CYCLE_3;
	  transferBytes[11] = CH_2_CYCLE_3;
	  transferBytes[12] = PLACEHOLDER_4;
	  transferBytes[13] = CH_1_CYCLE_4;
	  transferBytes[14] = CH_2_CYCLE_4;
	  transferBytes[15] = PLACEHOLDER_5;
	  transferBytes[16] = CH_1_CYCLE_5;
	  transferBytes[17] = CH_2_CYCLE_5;
	  transferBytes[18] = PLACEHOLDER_6;
	  transferBytes[19] = CH_1_CYCLE_6;
	  transferBytes[20] = CH_2_CYCLE_6;
	  transferBytes[21] = PLACEHOLDER_7;
	  transferBytes[22] = CH_1_CYCLE_7;
	  transferBytes[23] = CH_2_CYCLE_7;
	  transferBytes[24] = PLACEHOLDER_8;
	  transferBytes[25] = CH_1_CYCLE_8;
	  transferBytes[26] = CH_2_CYCLE_8;
	  transferBytes[27] = PLACEHOLDER_9;
	  transferBytes[28] = CH_1_CYCLE_9;
	  transferBytes[29] = CH_2_CYCLE_9;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_CYCLE_SETUP_0_9, 30, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

	  /* Change the Allocation of channels into cycles 10-17 */
	  /* Memory Map Position 0xB0 - 0xCA */
	  transferBytes[0] = PLACEHOLDER_10;
	  transferBytes[1] = CH_1_CYCLE_10;
	  transferBytes[2] = CH_2_CYCLE_10;
	  transferBytes[3] = PLACEHOLDER_11;
	  transferBytes[4] = CH_1_CYCLE_11;
	  transferBytes[5] = CH_2_CYCLE_11;
	  transferBytes[6] = PLACEHOLDER_12;
	  transferBytes[7] = CH_1_CYCLE_12;
	  transferBytes[8] = CH_2_CYCLE_12;
	  transferBytes[9] = PLACEHOLDER_13;
	  transferBytes[10] = CH_1_CYCLE_13;
	  transferBytes[11] = CH_2_CYCLE_13;
	  transferBytes[12] = PLACEHOLDER_14;
	  transferBytes[13] = CH_1_CYCLE_14;
	  transferBytes[14] = CH_2_CYCLE_14;
	  transferBytes[15] = PLACEHOLDER_15;
	  transferBytes[16] = CH_1_CYCLE_15;
	  transferBytes[17] = CH_2_CYCLE_15;
	  transferBytes[18] = PLACEHOLDER_16;
	  transferBytes[19] = CH_1_CYCLE_16;
	  transferBytes[20] = CH_2_CYCLE_16;
	  transferBytes[21] = PLACEHOLDER_17;
	  transferBytes[22] = CH_1_CYCLE_17;
	  transferBytes[23] = CH_2_CYCLE_17;
	  while(HAL_GPIO_ReadPin(GPIOC,Ready_Pin));// Waits for the ready line to be low
	  writeByte(IQS7211A_MM_CYCLE_SETUP_10_17, 24, transferBytes);
	  HAL_Delay(1);//This is just hear to prevent another read/write byte starting immediately as this one ends corrupting data

}

