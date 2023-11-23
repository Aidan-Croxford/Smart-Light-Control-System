
#ifndef INC_SYSTENSTATES_H_
#define INC_SYSTENSTATES_H_


typedef enum{
Flashlight=1,
Emergency=2,
Mood=3

}SystemState;

typedef enum{
Strobe=1,
SOS=2,
custom_message=3,

}EmergencyMode;

typedef enum{
NoGesture=0,
Tap=1,
Hold=2,
SwipeLeft=4,
SwipeRight=8,
SwipeUp=16,
SwipeDown=32


}Gesture;

#endif /* INC_SYSTENSTATES_H_*/
