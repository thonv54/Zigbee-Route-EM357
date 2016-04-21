
#ifndef UART_TASK_H
#define UART_TASK_H


//---------------------Define------------------//
#define NORMAL_TIMEOUT	30				//50ms
#define NormalTimeOut 	0xFF            // timeout off UART message send is NormalTimeOut*MinimumTimeOut
#define MinimumTimeOut  1000			//MinimumTimeOut for lag value 2s

//------------------------------Level Controler Command----------------//
typedef struct{
	int8u Length;
	int8u Id;
	int8u Data;
	int8u CheckXor;
	int8u TimeOut;

}UartSendLevelControlDataBuffer;


//------------------------------Send Switch Control Command----------------//
typedef struct{
	int8u Length;
	int8u Id;
	int8u Data;
	int8u CheckXor;
	int8u TimeOut;

}UartSendSwitchControlDataBuffer;

typedef enum{
	NoChange			=	0,
	OffRelay			=	1,
	OnRelay				=	2,
	ToggleRelay			=	3,
} SwitchStateEnum;

//------------------------------Send Attribute Setup Command----------------//
typedef struct{
	int8u Length;
	int8u Id;
	int8u AttribteId;
	int8u Value[5];
	int8u CheckXor;
	int8u TimeOut;

}UartSendAttributeSetupDataBuffer;

typedef enum{
	Save		=	2,
	NotSave     =	1,
}StoreAttributeOptionEnum;
typedef enum{
	SaveState	=	1,
	DimSpeed	=	2,
	CurtainTime =	3,
}SetupAttributeIdEnum;
typedef enum{
	McReady     = 0   ,
	McNotReady  = 1  ,
	McChecking	= 2   ,
}McReadyStatusEnum;
//------------------------------Send MC Timer Calibrate Command----------------//
typedef struct {
    int8u Length;
    int8u Id;
	int8u Inverval;
	int8u TotalTimes;
	int8u TimesCounter;
	int8u CheckXor;
    int8u TimeOut;
}UartSendMcTimerCalibrateDataBuffer;

//------------------------------Send MC Clock Calibrate Command----------------//
typedef struct {
    int8u Length;
    int8u Id;
	int8u Data[16];
	int8u CheckXor;
    int8u TimeOut;
}UartSendMcClockCalibrateDataBuffer;

//------------------------------Send Check MC Ready Command----------------//
typedef struct {
    int8u Length;
    int8u Id;
	int8u Data[16];
	int8u CheckXor;
    int8u TimeOut;
}UartSendCheckMcReadyDataBuffer;

//----------------------------------Send Special Led Command------------------------//
typedef struct {
    int8u Length;
    int8u Id;
    int8u State;
	int8u Extra;
	int8u CheckXor;
    int8u TimeOut;
}UartSendSpecialLedDataBuffer;

typedef enum {
	NormalState 	= 0,		// trang thai cua cong tac
	OffState 		= 1,        // led tat
	PinkState 		= 2,        // led chuyen hong
	BlinkState 		= 3,        // led nhay
}LedStateEnum;              // dung chung cho trang thai dac biet va trang thai sau khi nhay led



typedef struct {
	int8u Reverse 		: 	1;
	int8u RelayStatus 	: 	2;
	int8u LedState 		:	2;
	int8u BlinkTime 	: 	3;
}BlinkStateExtra;



//--------------------Protocol-----------------//
void UartScanCommand(void);
void UartSendMcClockCalibrate(void);
void UartSendMcTimerCalibrate(int8u inverval, int8u totalTimes, int8u timesCounter);

void UartSendSpecialLedMark (int8u LedNumberMark, int8u State);
void UartSendSpecialLedNumber (int8u LedNumber, int8u State);
void UartSendPinkLed(int8u LedNumberMark);
void UartSendOffLed(int8u LedNumberMark);
void UartSendBlinkLed(int8u LedNumberMark, int8u BlinkTime);
void UartSendToggleLed(int8u LedNumberMark);
void UartSendRefreshAllLed(void);
void UartSendRefreshLedNumber(int8u LedNumber);

void UartSendSwitchControlNumber(int8u SwitchNumber, int8u State);
void UartSendSwitchControlStateMark(int8u SwitchStateMark);

void UartSendCheckMcReady(void);

void UartSendDimLevel (int8u currentDimLevel);

unsigned char GetLastTxUartCmd(void);
void UartSendCommand(void);



#endif