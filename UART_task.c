





//-----------------------------Khai bao thu vien---------------------//


#include "app/framework/include/af.h"
#include "task.h"
#include "ButtonExe.h"
#include "UART_task.h"
#include "halDelay.h"
#include "ConfigDevice.h"

//-----------------------------Khai bao bien-------------------------//
int8u UART_PACKET_KEY[2]	=	{0x4C,0x4D};



int8u UartTxCommandBuff[256];
#define MAX_RX_BUFFER  256
int16u UartTxCommandStartWaitTimer = 0;
int16u UartTxCommandCurrentTimeOut = 0;



int8u UartRxDataStep = 0;
int8u UartRxPacketLength;
int8u UartRxPacketCheckXor = 0;
int8u UartRxCommandData[64] = {0};
int8u UartRxCurrentLength = 0;

//extern int8u McReadyStatus;



//-------------------------------Enum------------------------------//


typedef enum UartRxCommand_ID{
	RelayStatusChange 	= 0xA0,
	CurtainLevelChange 	= 0xA1,
	DimmerLevelChange 	= 0xA2,
	FanLevelChange 	  	= 0x03,
	TouchButtonHold 	= 0xA7,
	McTimmerCalibrate	= 0xAE,
	CheckMcReady		= 0xD0,
	McClockCalibrate	= 0xD1,
};



//-------------------------------CallTask------------------------------//


//------------------------CallTaskRelayChangeCommand-------------------//
typedef enum RelayStateCmd{
	RelayOff			= 0,
	RelayOn				= 1,
};



void CallTaskRelayChangeCommand( unsigned char *Data){
     int8u i;
	 int8u RelayState = Data[1];
	 int8u CurrentState[4] = {0};
	 for(i=0;i<4;i++){
		emberAfReadServerAttribute(2*i+1, ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,
						&CurrentState[i], sizeof(CurrentState[i]));
		if(CurrentState[i] != ((RelayState >> i) & 0x01)){
			int8u Relay;
			//call button press call back;
			 CallButtonPress(i);
			// and always wtite relay attribute
			 switch((RelayState >> i) & 0x01){
				 case RelayOn:
				 {
					Relay = 1;
					emberAfWriteServerAttribute(2*i+1,
						ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID, &Relay, sizeof(Relay));
				 }
					break;
				 case RelayOff:
				 {
					Relay = 0;
					emberAfWriteServerAttribute(2*i+1,
						ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID, &Relay, sizeof(Relay));
				 }
					break;
				default:
					break;
			}
			EmberNetworkStatus NetworkStatus;
			NetworkStatus = emberAfNetworkState();

			if(NetworkStatus == EMBER_JOINED_NETWORK) {
                EmberStatus sendStatus;

		//	 	Send to Bind Device
				sendStatus = SendViaBindingTable(2*i+1,Relay,OnOffType);

		//		Send On-Off AttributeResponse, Zipato stupid update :D-----------------------------//
				SendOnOffControlReadAttribute(2*i+1,Relay);
			}

		// on-off led

			if(Relay == 1){
				CommonSetLed(i+1);
			}
			else{
				CommonClearLed(i+1);
			}
		}
	}
}

//------------------------CallTaskLevelControlCommand-------------------//


void CallTaskDimmerLevelChangeCommand(int8u *Data){
	int8u DimmerLevel;
	DimmerLevel = Data[1];


    int8u CurrentDimmerLevel;
	emberAfReadServerAttribute(DimmerInClusterEndpoint, ZCL_LEVEL_CONTROL_CLUSTER_ID,ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
				&CurrentDimmerLevel, sizeof(CurrentDimmerLevel));
	if(CurrentDimmerLevel != DimmerLevel) {

// 		write attribute
		emberAfWriteServerAttribute(DimmerInClusterEndpoint,ZCL_LEVEL_CONTROL_CLUSTER_ID,
									ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
									&DimmerLevel, sizeof(DimmerLevel));
		int8u OnOffValue;
		if(DimmerLevel == 0){
			OnOffValue = 0;
			emberAfWriteServerAttribute(DimmerInClusterEndpoint,ZCL_ON_OFF_CLUSTER_ID,
									ZCL_ON_OFF_ATTRIBUTE_ID,
									&OnOffValue, sizeof(OnOffValue));
		}
		else{
			OnOffValue = 1;
			emberAfWriteServerAttribute(DimmerInClusterEndpoint,ZCL_ON_OFF_CLUSTER_ID,
						ZCL_ON_OFF_ATTRIBUTE_ID,
						&OnOffValue, sizeof(OnOffValue));
		}
//	 	Send to Bind Device
		EmberStatus sendStatus;
		sendStatus = SendViaBindingTable(DimmerInClusterEndpoint,DimmerLevel,LevelWithOnOffType);

//		Send Level AttributeResponse, Zipato stupid update :D-----------------------------//
		SendLevelControlReadAttribute(DimmerInClusterEndpoint,DimmerLevel);
//		Send On-Off AttributeResponse, Zipato stupid update :D-----------------------------//
		SendOnOffControlReadAttribute(DimmerInClusterEndpoint,OnOffValue);
	}
}



//------------------------CallTaskMcClockCalibrate-------------------//



void CallTaskMcClockCalibrate(int8u *Data){
	int8u DataCheckKey[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
										0xBB,0xCC,0xDD,0xEE,0xFF};
	if(MEMCOMPARE(&Data[1], DataCheckKey, 0) == 0){
		UartSendMcClockCalibrate();
	}
}
//------------------------CallTaskCheckMcReady-------------------//
// This Uart Command Indicator MC StartUp, or EM reset end StartUp
void CallTaskCheckMcReady(int8u *Data){
	EmberNetworkStatus NetworkStatus;
	int8u DataCheckKey[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
										0xBB,0xCC,0xDD,0xEE,0xFF};
	if(MEMCOMPARE(&Data[1], DataCheckKey, 0) == 0){

		UartSendBlinkLed(0x0F, 2);
#ifdef SWITCH_TYPE
		UartSendSwitchControlStateMark(0x00);  // Get switch state after reset
#endif

		NetworkStatus = emberAfNetworkState();
		if(NetworkStatus == EMBER_JOINED_NETWORK) {
			UartSendBlinkLed(0x0F, 1);
//		This Command is EM after reset, so all startup code can be set here.
			GetHcActiveEndPoint();
		}
		else{
		    CallJoinTask(0xFF,3000 + ((int8u)halCommonGetRandom()<<2));
		}
	}
}
//------------------------CallTaskTouchButtonHold--------------------//
typedef enum ButtonHoldCmd{
	ButtonUnpress		= 0,
	ButtonShortHold		= 1,
	ButtonLongHold		= 2,
    ButtonResetHold		= 3,
};
void CallTaskTouchButtonHold(int8u *Data){
	int8u ButtonPress = Data[1] + 1;
	int8u ButtonHoldType = Data[2];

	switch (ButtonHoldType & 0x03){
		case ButtonUnpress:
			CallButtonRelease(ButtonPress);
		break;
		case ButtonShortHold:
			CallButtonShortHold(ButtonPress);
		break;
		case ButtonLongHold:
			CallButtonLongHold(ButtonPress);
		break;
		case ButtonResetHold:
		break;
		default:
		break;

	}
}
//------------------------CallTaskCalibrateTimmer--------------------//
void CallTaskMcTimmerCalibrate(int8u *Data){
	int8u Inverval = Data[1];
	int8u Times	= Data[2];
	CallMcTimmerCalibrateTask(Inverval,Times);

}

//----------------------------CallTask--------------------------------//

void CallUartTask(int8u *UartPacketCommand){
	int8u UartPacketID = UartPacketCommand[0];

	switch(UartPacketID){
		case RelayStatusChange:
			CallTaskRelayChangeCommand(UartPacketCommand);
			break;
		case CurtainLevelChange:
			break;
		case DimmerLevelChange:
			CallTaskDimmerLevelChangeCommand(UartPacketCommand);
			break;
		case FanLevelChange:
			break;
		case TouchButtonHold:
			CallTaskTouchButtonHold(UartPacketCommand);
			break;
		case McTimmerCalibrate:
			CallTaskMcTimmerCalibrate(UartPacketCommand);
			break;
		case McClockCalibrate:
			CallTaskMcClockCalibrate(UartPacketCommand);
			break;
		case CheckMcReady:
			CallTaskCheckMcReady(UartPacketCommand);
			break;
		default:
			break;
	}
}




void UartScanCommand(void){
	int16u NumberOfByteReCeiver;
	int8u ReadSerialData;
	int8u ReadStatus;


	NumberOfByteReCeiver = emberSerialReadAvailable (1);
	if(NumberOfByteReCeiver > 0){
		ReadStatus = emberSerialReadByte(1, &ReadSerialData);
		{

			emberSerialWriteData(1,&ReadSerialData,NumberOfByteReCeiver) ;

			if(ReadStatus == EMBER_SUCCESS){

				switch (UartRxDataStep){
				case 0:
					if(ReadSerialData == 0x4C){
						UartRxDataStep = 1;
					}
					else{
						UartRxDataStep = 0;
					}
				break;
				case 1:
					if(ReadSerialData == 0x4D){
						UartRxDataStep = 2;
					}
					else{
						UartRxDataStep = 0;
					}
				break;
				case 2:
					UartRxPacketLength = ReadSerialData;
					if(UartRxPacketLength >40){
						UartRxDataStep = 0;
					}
					else{
						UartRxDataStep = 3;
					}
				break;
				case 3:
					if(UartRxCurrentLength < UartRxPacketLength-1){
						UartRxCommandData[UartRxCurrentLength] =  ReadSerialData;
						UartRxCurrentLength ++;
					}
					else{
						int8u PacketCheckXor;
						int8u CheckXor =0;
						PacketCheckXor =  ReadSerialData;
						{
							for(int8u j=0;j < UartRxPacketLength-1;j++){
								CheckXor = CheckXor ^ UartRxCommandData[j];
							}
						}
						if(PacketCheckXor == CheckXor){
							CallUartTask(UartRxCommandData);
						}
						UartRxCurrentLength = 0;
						UartRxDataStep = 0;
					}
				break;
					default:
				break;

				}
			}
		}
	}
}



//--------------------------------------------UART Send Command--------------------------//


typedef enum UartTxCommand_ID{
	SwitchControlCmdId		=	0xB0,
	DimmerLevelControlCmdID =	0xB2,
	ChangeSpecialLedTxCmdId	=	0xB7,
	AttributeSetupCmdId		=	0xBC,
	McClockCalibrateCmdId  = 	0xC1,
	McReadyCmdId			=	0xC0,
	McTimerCalibrateCmdId	=	0xBE,
};

//----------------------------UART Send Command Utility-----------------------//

unsigned char GetLastTxUartCmd(void){
    unsigned char Head = 0;
    unsigned char Length = 0;
    while(UartTxCommandBuff[Head] != 0){
        if(UartTxCommandBuff[Head] != 0){
            Length = UartTxCommandBuff[Head];
            Head = Head+Length+2;
        }
    }
    return Head;
}

//------------------------------Send Dimmer Level Control Command----------------//
 /*
Mau ban tin:

CommandID			Dimmer Level
1byte				1 Byte
0xB1
*/
void UartSendDimLevel (int8u currentDimLevel) {
	UartSendLevelControlDataBuffer UartSendLevelControlData;

	UartSendLevelControlData.Length = 3;
	UartSendLevelControlData.Id = DimmerLevelControlCmdID;
	UartSendLevelControlData.Data = currentDimLevel;
    UartSendLevelControlData.CheckXor =  UartSendLevelControlData.Id ^ UartSendLevelControlData.Data;
	UartSendLevelControlData.TimeOut = NormalTimeOut;

	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendLevelControlData,
			sizeof(UartSendLevelControlDataBuffer));
}

//------------------------------Send Switch Control Command----------------//
 /*
Mau ban tin:

CommandID			Switch State
1byte				11Byte
0xB0
*/



// State off Relay Use SwitchStateEnum
void UartSendSwitchControlNumber(int8u SwitchNumber, int8u State){
	UartSendSwitchControlDataBuffer UartSendSwitchControlData;

	UartSendSwitchControlData.Length = 3;
	UartSendSwitchControlData.Id = SwitchControlCmdId;
	UartSendSwitchControlData.Data = (State << 2*(SwitchNumber-1));
    UartSendSwitchControlData.CheckXor =  UartSendSwitchControlData.Id ^ UartSendSwitchControlData.Data;
	UartSendSwitchControlData.TimeOut = NormalTimeOut;

	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendSwitchControlData,
			sizeof(UartSendSwitchControlDataBuffer));
}

void UartSendSwitchControlMark(int8u SwitchMark, int8u State){
	int8u i;
	UartSendSwitchControlDataBuffer UartSendSwitchControlData;

	for(i=0;i<4;i++){
		if(((SwitchMark >> i) & 0x01) == 1){
			UartSendSwitchControlData.Data = UartSendSwitchControlData.Data | (State << 2*i);
		}
	}
	UartSendSwitchControlData.Length = 3;
	UartSendSwitchControlData.Id = SwitchControlCmdId;
	UartSendSwitchControlData.CheckXor = UartSendSwitchControlData.Id ^ UartSendSwitchControlData.Data;
	UartSendSwitchControlData.TimeOut = NormalTimeOut;
	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendSwitchControlData,
			sizeof(UartSendSwitchControlDataBuffer));
}
void UartSendSwitchControlStateMark(int8u SwitchStateMark){
	UartSendSwitchControlDataBuffer UartSendSwitchControlData;

	UartSendSwitchControlData.Length = 3;
	UartSendSwitchControlData.Id = SwitchControlCmdId;
	UartSendSwitchControlData.Data = SwitchStateMark;
    UartSendSwitchControlData.CheckXor =  UartSendSwitchControlData.Id ^ UartSendSwitchControlData.Data;
	UartSendSwitchControlData.TimeOut = NormalTimeOut;
	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendSwitchControlData,
			sizeof(UartSendSwitchControlDataBuffer));
}

//------------------------------Send Attribute Setup Command----------------//
 /*
Mau ban tin:

CommandID			Attribute			Value
1byte				1Byte               1 Byte
0xBC
*/

// Option of Store value use StoreAttributeEnum
// AttribteId use SetupAttributeIdEnum
void UartSendStoreAttribute(int8u SwitchMark, int8u Option){
	UartSendAttributeSetupDataBuffer UartSendAttributeSetupData;
	int8u i;
	for(i=0;i<4;i++){
		if(((SwitchMark<<i) & 0x01) == 1){
			UartSendAttributeSetupData.Value[0] = UartSendAttributeSetupData.Value[0] | (Option<<(2*i));
		}
	}


	UartSendAttributeSetupData.Length = 8;
	UartSendAttributeSetupData.Id = AttributeSetupCmdId;
	UartSendAttributeSetupData.AttribteId = SaveState;
	UartSendAttributeSetupData.CheckXor = UartSendAttributeSetupData.Value[0] ^ UartSendAttributeSetupData.AttribteId
		^ UartSendAttributeSetupData.Id;
    UartSendAttributeSetupData.TimeOut = NormalTimeOut;

    MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendAttributeSetupData,
			sizeof(UartSendAttributeSetupDataBuffer));
}




//------------------------------Send MC Clock Calibrate Command----------------//
 /*
Mau ban tin:

CommandID			Calibrate Data
1byte				16Byte
0xC1				0x00 ->0xFF
*/



void UartSendMcClockCalibrate(void){
	UartSendMcClockCalibrateDataBuffer UartSendMcClockCalibrateData;
	int8u DataCheckKey[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
										0xBB,0xCC,0xDD,0xEE,0xFF};
	UartSendMcClockCalibrateData.Length = 18;
	UartSendMcClockCalibrateData.Id		= McClockCalibrateCmdId;
	MEMCOPY(&UartSendMcClockCalibrateData.Data, DataCheckKey, sizeof(DataCheckKey));
	UartSendMcClockCalibrateData.CheckXor = McClockCalibrateCmdId;
	UartSendMcClockCalibrateData.TimeOut = NormalTimeOut;

	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendMcClockCalibrateData,
			sizeof(UartSendMcClockCalibrateData));
}


//----------------------------------Send Special Led Command------------------------//
 /*
Mau ban tin:

CommandID			State			Extra
1byte				1Byte			1Byte
0xB7
*/


// State off Special Led use LedStateEnum
// State off Led after Blink use LedStateEnum
void UartSendRefreshAllLed(void){
	UartSendSpecialLedMark(0x0F, 0);
}
void UartSendRefreshLedNumber(int8u LedNumber){
	UartSendSpecialLedNumber(LedNumber, 0);
}
void UartSendSpecialLedMark (int8u LedNumberMark, int8u State){
	UartSendSpecialLedDataBuffer UartSendSpecialLedMarkData;
	int8u i;

	for(i=0;i<4;i++){
		if(((LedNumberMark >> i) & 0x01) == 1){
			UartSendSpecialLedMarkData.State = UartSendSpecialLedMarkData.State | (State << 2*i);
		}
	}
	UartSendSpecialLedMarkData.Extra = 0;
	UartSendSpecialLedMarkData.Length = 4;
	UartSendSpecialLedMarkData.Id = ChangeSpecialLedTxCmdId;
	UartSendSpecialLedMarkData.CheckXor =	UartSendSpecialLedMarkData.Id ^ UartSendSpecialLedMarkData.State ^
		UartSendSpecialLedMarkData.Extra;

	UartSendSpecialLedMarkData.TimeOut = NormalTimeOut;

   	MEMCOPY(&UartTxCommandBuff[GetLastTxUartCmd()],&UartSendSpecialLedMarkData,
			sizeof(UartSendSpecialLedDataBuffer));

}


void UartSendSpecialLedNumber (int8u LedNumber, int8u State){
	UartSendSpecialLedDataBuffer UartSendSpecialLedMarkData;


	UartSendSpecialLedMarkData.State =  (State << (2*(LedNumber -1)));
	UartSendSpecialLedMarkData.Extra = 0;
	UartSendSpecialLedMarkData.Length = 4;
	UartSendSpecialLedMarkData.Id = ChangeSpecialLedTxCmdId;
	UartSendSpecialLedMarkData.CheckXor =	UartSendSpecialLedMarkData.Id ^ UartSendSpecialLedMarkData.State ^
		UartSendSpecialLedMarkData.Extra;
    UartSendSpecialLedMarkData.TimeOut = NormalTimeOut;
	MEMCOPY(&UartTxCommandBuff[GetLastTxUartCmd()],&UartSendSpecialLedMarkData,
			sizeof(UartSendSpecialLedDataBuffer));

}



void UartSendPinkLed(int8u LedNumberMark){
	UartSendSpecialLedDataBuffer UartSendSpecialLedMarkData;

	int8u i;

	for(i=0;i<4;i++){
		if(((LedNumberMark >> i) & 0x01) == 1){
			UartSendSpecialLedMarkData.State = UartSendSpecialLedMarkData.State | (PinkState << 2*i);
		}
	}
	UartSendSpecialLedMarkData.Extra = 0;
	UartSendSpecialLedMarkData.Length = 4;
	UartSendSpecialLedMarkData.Id = ChangeSpecialLedTxCmdId;
	UartSendSpecialLedMarkData.CheckXor =	UartSendSpecialLedMarkData.Id ^ UartSendSpecialLedMarkData.State ^
		UartSendSpecialLedMarkData.Extra;

	UartSendSpecialLedMarkData.TimeOut = NormalTimeOut;
    MEMCOPY(&UartTxCommandBuff[GetLastTxUartCmd()],&UartSendSpecialLedMarkData,
			sizeof(UartSendSpecialLedDataBuffer));
}


void UartSendOffLed(int8u LedNumberMark){
	UartSendSpecialLedDataBuffer UartSendSpecialLedMarkData;

	int8u i;

	for(i=0;i<4;i++){
		if(((LedNumberMark >> i) & 0x01) == 1){
			UartSendSpecialLedMarkData.State = UartSendSpecialLedMarkData.State | (OffState << 2*i);
		}
	}

	UartSendSpecialLedMarkData.Extra = 0;
	UartSendSpecialLedMarkData.Length = 4;
	UartSendSpecialLedMarkData.Id = ChangeSpecialLedTxCmdId;
	UartSendSpecialLedMarkData.CheckXor =	UartSendSpecialLedMarkData.Id ^ UartSendSpecialLedMarkData.State ^
		UartSendSpecialLedMarkData.Extra;
	UartSendSpecialLedMarkData.TimeOut = NormalTimeOut;
	MEMCOPY(&UartTxCommandBuff[GetLastTxUartCmd()],&UartSendSpecialLedMarkData,
			sizeof(UartSendSpecialLedDataBuffer));
}



void UartSendBlinkLed(int8u LedNumberMark, int8u BlinkTime){
	UartSendSpecialLedDataBuffer UartSendSpecialLedMarkData;

    BlinkStateExtra BlinkStateExtraData;
	int8u i;

	for(i=0;i<4;i++){
		if(((LedNumberMark >> i) & 0x01) == 1){
			UartSendSpecialLedMarkData.State = UartSendSpecialLedMarkData.State | (BlinkState << 2*i);
		}
	}
	BlinkStateExtraData.BlinkTime = BlinkTime;
	BlinkStateExtraData.LedState = NormalState;
	BlinkStateExtraData.RelayStatus = NoChange;

	MEMCOPY(&UartSendSpecialLedMarkData.Extra, &BlinkStateExtraData, sizeof(UartSendSpecialLedMarkData.Extra));
	UartSendSpecialLedMarkData.Length = 4;
	UartSendSpecialLedMarkData.Id = ChangeSpecialLedTxCmdId;
	UartSendSpecialLedMarkData.CheckXor =	UartSendSpecialLedMarkData.Id ^ UartSendSpecialLedMarkData.State ^
		UartSendSpecialLedMarkData.Extra;
	UartSendSpecialLedMarkData.TimeOut = BlinkTime;
	MEMCOPY(&UartTxCommandBuff[GetLastTxUartCmd()],&UartSendSpecialLedMarkData,
			sizeof(UartSendSpecialLedDataBuffer));
}
void UartSendToggleLed(int8u LedNumberMark){
	UartSendBlinkLed(LedNumberMark,0);
}

//----------------------------------Send Check MC Ready------------------------//
 /*
Mau ban tin:

CommandID			 Data
1byte				16Byte
0xB0				0x00 -> 0xFF
*/
void UartSendCheckMcReady(void){
	UartSendCheckMcReadyDataBuffer UartSendCheckMcReadyData;
	int8u DataCheckKey[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
										0xBB,0xCC,0xDD,0xEE,0xFF};
	UartSendCheckMcReadyData.Length = 18;
	UartSendCheckMcReadyData.Id		= McReadyCmdId;
	MEMCOPY(&UartSendCheckMcReadyData.Data, DataCheckKey, sizeof(DataCheckKey));
	UartSendCheckMcReadyData.CheckXor = McReadyCmdId;
	UartSendCheckMcReadyData.TimeOut = NormalTimeOut;

	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendCheckMcReadyData,
			sizeof(UartSendCheckMcReadyData));
//	McReadyStatus = McChecking;
}


//----------------------------------Send MC Timmer Calibrate ------------------------//
 /*
Mau ban tin:

CommandID			 Interval		TotalTime			TimeCounter
1byte				1Byte           1byte                1byte
0xBE
*/
void UartSendMcTimerCalibrate(int8u inverval, int8u totalTimes, int8u timesCounter){
	UartSendMcTimerCalibrateDataBuffer UartSendMcTimerCalibrateData;

	UartSendMcTimerCalibrateData.Length = 5;
	UartSendMcTimerCalibrateData.Id		= McTimerCalibrateCmdId;
	UartSendMcTimerCalibrateData.Inverval = inverval;
	UartSendMcTimerCalibrateData.TotalTimes = totalTimes;
	UartSendMcTimerCalibrateData.TimesCounter= timesCounter;
	UartSendMcTimerCalibrateData.CheckXor = McTimerCalibrateCmdId ^ inverval ^ totalTimes ^ timesCounter;
	UartSendMcTimerCalibrateData.TimeOut = NormalTimeOut;

	MEMCOPY(&UartTxCommandBuff [GetLastTxUartCmd()], &UartSendMcTimerCalibrateData,
			sizeof(UartSendMcTimerCalibrateDataBuffer));
//	McReadyStatus = McChecking;
}




//---------------------------------Check Uart Status And Send Quere Data Buffer-------------------------------//
typedef enum {
	CmdReady,
	CmdBusy,
}uartSendCommandStatus;

int8u CheckWaitTimeOut(void){

	if(UartTxCommandStartWaitTimer !=0 ){
		if(CommonGetDurationTime(UartTxCommandStartWaitTimer) >= UartTxCommandCurrentTimeOut){
			//clear timer
			UartTxCommandStartWaitTimer = 0;
			return CmdReady;
		}
		else{
			return CmdBusy;
		}
	}
	else{
		return CmdReady;
	}
}
void UartSendCommand(void){
	//	Check buff
	if(UartTxCommandBuff[0] != 0){
		//	Check Timeout


		switch (CheckWaitTimeOut()){
			case CmdBusy:
			break;
			case CmdReady:
				//	Send buff
				if(UartTxCommandBuff[UartTxCommandBuff[0]+1] == NormalTimeOut){
					UartTxCommandCurrentTimeOut = NORMAL_TIMEOUT;
				}
				else{
					UartTxCommandCurrentTimeOut = UartTxCommandBuff[UartTxCommandBuff[0]+1] * MinimumTimeOut;
				}
				emberSerialWriteData(1,UART_PACKET_KEY,2);
				emberSerialWriteData(1,UartTxCommandBuff, (UartTxCommandBuff[0]+1));
				MEMCOPY(UartTxCommandBuff, &UartTxCommandBuff[UartTxCommandBuff[0]+2],
						(MAX_RX_BUFFER - (UartTxCommandBuff[0]+2)));
				UartTxCommandStartWaitTimer = (int16u)halCommonGetInt32uMillisecondTick();
			break;
			default:
			break;
		}
	}
}

