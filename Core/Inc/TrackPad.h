/*
 * TrackPad.h
 *
 *  Created on: Apr 26, 2023
 *      Author: Aidan
 */

#ifndef INC_TRACKPAD_H_
#define INC_TRACKPAD_H_



void getProductNum();
void readByte(uint8_t MemAddress,uint8_t numBytes, uint8_t data[]);
void writeByte(uint8_t MemAddress,uint8_t numBytes, uint8_t data[]);
void writeMM();




#endif /* INC_TRACKPAD_H_ */
