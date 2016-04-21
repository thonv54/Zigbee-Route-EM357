
#ifndef BUTTON_EXE_H
#define BUTTON_EXE_H


//---------------------Define------------------//

#define CallButtonShortHold 	beButtonPress2Time
#define CallButtonLongHold		beButtonPress3Time
#define CallButtonPress		beButtonPress
#define CallButtonRelease		beButtonRealease





//--------------------Protocol-----------------//

void beButtonPress (unsigned char ButtonNumber);
void beButtonPress3Time(int8u ButtonNumber);
void beButtonPress2Time(int8u ButtonNumber);
void beButtonRealease(int8u ButtonNumber);


void CommonSetLed(int8u LedNumber);
void CommonClearLed(int8u LedNumber);
void CommonLedRefresh(void);

#endif