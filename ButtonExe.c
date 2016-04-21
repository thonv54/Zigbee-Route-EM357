



//-----------------------------Khai bao thu vien---------------------//


#include "app/framework/include/af.h"
#include "ButtonExe.h"
#include "task.h"
#include "UART_task.h"
#include "halDelay.h"

//-----------------------------Khai bao bien-------------------------//

int16u ReleaseLongHoldTime = 0;
int8u ButtonLongHoldFlag;


//---------------------Enum--------------------//



//---------------------Struct------------------//



//------------------------------Xu li chuong trinh-------------------//





void beButtonPress (unsigned char ButtonNumber){
}

void beButtonPress3Time(int8u ButtonNumber){
//	EmberNetworkStatus NetworkStatus;
//	NetworkStatus = emberAfNetworkState();
//
//
//	if(NetworkStatus == EMBER_NO_NETWORK){
////		if no network -> network join
////		CallJoinTask(10,6000);
//        ClearAllBlinkTask();
//		UartSendBlinkLed(0x0F,2);
//	}
//	else if (NetworkStatus == EMBER_JOINED_NETWORK){
////		if had network -> network leave
//		emberLeaveNetwork();
////		include clear bindTable;
//		emberClearBindingTable();
////		Clear all Blinkled before new Blink
//
////		Led Flash to display Network Leave success
//		UartSendBlinkLed(0x0F,2);
//
//	}
	UartSendPinkLed(0x0F);
	ReleaseLongHoldTime =  (int16u)halCommonGetInt32uMillisecondTick();
	ButtonLongHoldFlag = 1;

}
void beButtonRealease(int8u ButtonNumber){
	if(ButtonLongHoldFlag == 1){
		if(CommonGetDurationTime(ReleaseLongHoldTime) < 1000){
			emberLeaveNetwork();
			emberClearBindingTable();
//			UartSendBlinkLed(0x0F,2);
			CallResetMCU(500);
		}
		else{
			UartSendRefreshAllLed();
		}
	}
	ButtonLongHoldFlag  = 0;
}
void beButtonPress2Time(int8u ButtonNumber){
//		Creat Simple Bind Request ButtonNumber is the Endpoint Bind
// 		Send Pink State to Led
//		When Bind done clear Led by func ZDO response Bind Success call back.
//		emberAfSendEndDeviceBind(ButtonNumber);
//		UartSendSpecialLedNumber(ButtonNumber, PinkState);

}

void CommonSetLed(int8u LedNumber){

	switch (LedNumber){
		case 1: halSetLed(BOARDLED0);
			break;
		case 2: halSetLed(BOARDLED1);
			break;
		case 3: halSetLed(BOARDLED2);
			break;
		default:
			break;
	}
}

void CommonClearLed(int8u LedNumber){

	switch (LedNumber){
		case 1: halClearLed(BOARDLED0);
			break;
		case 2: halClearLed(BOARDLED1);
			break;
		case 3: halClearLed(BOARDLED2);
			break;
		default:
			break;
	}
}
void CommonLedRefresh(void){
	EmberAfStatus readStatus;
	int8u LedStatus;
	int8u i;
	for(i=1;i<4;i++){
		readStatus = emberAfReadServerAttribute(i,
			ZCL_ON_OFF_CLUSTER_ID, ZCL_ON_OFF_ATTRIBUTE_ID, &LedStatus, sizeof(LedStatus));
		if(readStatus == EMBER_ZCL_STATUS_SUCCESS){
			if(LedStatus == 1){
				CommonSetLed(i);
			}
			else{
				CommonClearLed(i);
			}
		}
	}
}


void TestUART(void){
	int16u NumberOfByteReCeiver;
	int8u ReadSerialData[128] ={0};
	int8u ReadStatus;

	NumberOfByteReCeiver = emberSerialReadAvailable (1);
	if(NumberOfByteReCeiver > 0){
		ReadStatus = emberSerialReadByte(1, ReadSerialData);
		if(ReadStatus == EMBER_SUCCESS){
			emberSerialWriteData(1,ReadSerialData, 1);
		}
	}
}



//------------------------------End File-------------------------//