/*
 * Morse.c
 *
 *  Created on: Mar 21, 2023
 *      Author: Aidan
 */


#include "String.h"
#include "Functions.h"
#include "stdint.h"
#include "usart.h"

extern uint8_t Custom_Length;

//All the morse characters that can be received
char A[3]=".-";//2
char B[4]="-..";
char C[5]="-.-.";
char D[4]="-..";
char E[2]=".";
char F[5]="..-.";
char G[4]="--.";
char H[5]="....";
char I[3]="..";
char J[5]=".---";
char K[4]="-.-";
char L[5]=".-..";
char M[3]="--";
char N[3]="-.";//2
char O[4]="---";
char P[5]=".--.";
char Q[5]="--.-";
char R[4]=".-.";
char S[4]="...";
char T[2]="-";
char U[4]="..-";
char V[4]="...-";//4
char W[4]=".--";
char X[5]="-..-";
char Y[5]="-.--";
char Z[5]="--..";

char ONE[6]=".----";
char TWO[6]="..---";
char THREE[6]="...--";
char FOUR[6]="....-";
char FIVE[6]=".....";
char SIX[6]="-....";
char SEVEN[6]="--...";
char EIGHT[6]="---..";
char NINE[6]="----.";
char ZERO[6]="-----";


extern char CustomMessage[20];



//Morse code lookup table
void MorseIndex(char c ){

switch(c){

case 'A':
	strcat(CustomMessage,A);
	Custom_Length=Custom_Length+sizeof(A)-1;
	break;
case 'B':
	strcat(CustomMessage,B);
	Custom_Length=Custom_Length+sizeof(B)-1;
	break;
case 'C':
	strcat(CustomMessage,C);
	Custom_Length=Custom_Length+sizeof(C)-1;
	break;
case 'D':
	strcat(CustomMessage,D);
	Custom_Length=Custom_Length+sizeof(D)-1;
	break;
case 'E':
	strcat(CustomMessage,E);
	Custom_Length=Custom_Length+sizeof(E)-1;
	break;
case 'F':
	strcat(CustomMessage,F);
	Custom_Length=Custom_Length+sizeof(F)-1;
	break;
case 'G':
	strcat(CustomMessage,G);
	Custom_Length=Custom_Length+sizeof(G)-1;
	break;
case 'H':
	strcat(CustomMessage,H);
	Custom_Length=Custom_Length+sizeof(H)-1;
	break;
case 'I':
	strcat(CustomMessage,I);
	Custom_Length=Custom_Length+sizeof(I)-1;
	break;
case 'J':
	strcat(CustomMessage,J);
	Custom_Length=Custom_Length+sizeof(J)-1;
	break;
case 'K':
	strcat(CustomMessage,K);
	Custom_Length=Custom_Length+sizeof(K)-1;
	break;
case 'L':
	strcat(CustomMessage,L);
	Custom_Length=Custom_Length+sizeof(L)-1;
	break;
case 'M':
	strcat(CustomMessage,M);
	Custom_Length=Custom_Length+sizeof(M)-1;
	break;
case 'N':
	strcat(CustomMessage,N);
	Custom_Length=Custom_Length+sizeof(N)-1;
	break;
case 'O':
	strcat(CustomMessage,O);
	Custom_Length=Custom_Length+sizeof(O)-1;
	break;
case 'P':
	strcat(CustomMessage,P);
	Custom_Length=Custom_Length+sizeof(P)-1;
	break;
case 'Q':
	strcat(CustomMessage,Q);
	Custom_Length=Custom_Length+sizeof(Q)-1;
	break;
case 'R':
	strcat(CustomMessage,R);
	Custom_Length=Custom_Length+sizeof(R)-1;
	break;
case 'S':
	strcat(CustomMessage,S);
	Custom_Length=Custom_Length+sizeof(S)-1;
	break;
case 'T':
	strcat(CustomMessage,T);
	Custom_Length=Custom_Length+sizeof(T)-1;
	break;
case 'U':
	strcat(CustomMessage,U);
	Custom_Length=Custom_Length+sizeof(U)-1;
	break;
case 'V':
	strcat(CustomMessage,V);
	Custom_Length=Custom_Length+sizeof(V)-1;
	break;
case 'W':
	strcat(CustomMessage,W);
	Custom_Length=Custom_Length+sizeof(W)-1;
	break;
case 'X':
	strcat(CustomMessage,X);
	Custom_Length=Custom_Length+sizeof(X)-1;
	break;
case 'Y':
	strcat(CustomMessage,Y);
	Custom_Length=Custom_Length+sizeof(Y)-1;
	break;
case 'Z':
	strcat(CustomMessage,Z);
	Custom_Length=Custom_Length+sizeof(Z)-1;
	break;
case '0':
	strcat(CustomMessage,"-----");
	Custom_Length=Custom_Length+5;
	break;
case '1':
	strcat(CustomMessage,ONE);
	Custom_Length=Custom_Length+sizeof(ONE)-1;
	break;
case '2':
	strcat(CustomMessage,TWO);
	Custom_Length=Custom_Length+sizeof(TWO)-1;
	break;
case '3':
	strcat(CustomMessage,THREE);
	Custom_Length=Custom_Length+sizeof(THREE)-1;
	break;
case '4':
	strcat(CustomMessage,FOUR);
	Custom_Length=Custom_Length+sizeof(FOUR)-1;
	break;
case '5':
	strcat(CustomMessage,FIVE);
	Custom_Length=Custom_Length+sizeof(FIVE)-1;
	break;
case '6':
	strcat(CustomMessage,SIX);
	Custom_Length=Custom_Length+sizeof(SIX)-1;
	break;
case '7':
	strcat(CustomMessage,SEVEN);
	Custom_Length=Custom_Length+sizeof(SEVEN)-1;
	break;
case '8':
	strcat(CustomMessage,EIGHT);
	Custom_Length=Custom_Length+sizeof(EIGHT)-1;
	break;
case '9':
	strcat(CustomMessage,NINE);
	Custom_Length=Custom_Length+sizeof(NINE)-1;
	break;
case ' ':
	strcat(CustomMessage," ");
	Custom_Length++;
	break;



}


}
