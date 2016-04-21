
//-----------------------------Khai bao thu vien---------------------//


#include "app/framework/include/af.h"
#include "halDelay.h"

//-----------------------------Khai bao bien-------------------------//




int16u CommonGetDurationTime(int16u CommonBeforeTimer){
	int16u DurationTime;
	int16u CurrentTime;
	CurrentTime = (int16u)halCommonGetInt32uMillisecondTick();

	if( CurrentTime >= CommonBeforeTimer){
		DurationTime = CurrentTime - CommonBeforeTimer;
	}
	else{
		DurationTime = 0xFFFF - CommonBeforeTimer + CurrentTime;
	}
	return DurationTime;
}

void __delay_ms(int32u n){
	int32u StartDelayTimer;
	StartDelayTimer = halCommonGetInt32uMillisecondTick();
	while(CommonGetDurationTime(StartDelayTimer) < n){
		halResetWatchdog();
	}
}



