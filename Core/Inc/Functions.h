/*
 * Functions.h
 *
 *  Created on: 18 Mar 2023
 *      Author: Aidan
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include "stdint.h"

void UART_Command(uint8_t CommandLength);
void Setup();
void SystemRefresh();/// Might be able to use this instead of StatusUpdate()
void DecodeMorse();//Decodes the custom message into its morse code
void ExecuteMorse(char *MorseCode);//Called to execute the morse code
void Slider_Update();
void debugLEDs();

void TrackPadInput();
void TrackPadSetup();
void test();



#endif /* INC_FUNCTIONS_H_ */
