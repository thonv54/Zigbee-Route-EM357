
#ifndef TASK_H
#define TASK_H


//---------------------Define------------------//

#define TASKDATASIZE  64


//---------------------Enum--------------------//
typedef enum TaskList{
    TaBlinkLed1 	= 			1,
    TaBlinkLed2 	= 			2,
    TaBlinkLed3 	= 			3,
	TaBlinkLed4		=		4,
	TaBlinkLedAll	=			5,
    TaButtonDetect 	= 			6,
    TaJoin        	=   		7,
	TaOnOffAttributeChange = 	8,
	TaResetMCU		=			9,
	TaUartSendDimLevel = 		10,
	TaMcTimerCalibrate = 		11,

}TaskListEnum;
typedef enum taskStatus{
    CompleteTask = 1,
    ContinuteTask = 2,
}taskStatusEnum;
typedef enum taskDulicateStatus{
    TaskOK = 0xFF,
}taskDulicateStatusEnum;
typedef enum taskClearStatus{
	TaskClearOK = 0xFF,
	TaskIdNotFound = 1,
}taskClearStatusEnum;
typedef enum BindCommandType{
   OnOffType			= 1,
   LevelType			= 2,
   LevelWithOnOffType 	= 3,
}BindCommandTypeEnum;

typedef struct {
  /** The AttributeID of on-off cluster */
  int16u AttributeID;
  /** The Status  */
  int8u Status;
  /** The DataType. */
  int8u DataType;
  /** Value. */
  int8u Value;

} EmberOnOffAttributeResponse;
#define	EmberOnOffAttributeResponseLength	5


//--------------------Protocol-----------------//

int8u SendViaBindingTable (int8u sourceEndpoint, int8u Value, int8u CommandType);
void SendLevelControlReadAttribute(int8u Endpoint, int8u Value);
void SendOnOffControlReadAttribute(int8u Endpoint, int8u Value);


unsigned char GetLastTask(void);
void TaskDelete (unsigned char head);
unsigned char FindTaskID(unsigned char TaskID);
unsigned char ClearTask(unsigned char taskID);

void CallBlinkLedTask (unsigned char n, unsigned short delayTime, unsigned short maxCnt);
unsigned char TaskBlinkLed (unsigned short *Data);
void ClearAllBlinkTask(void);

void CallButtonDetectTask(void);
unsigned char TaskButtonDetect (unsigned short *Data);

void CallJoinTask(unsigned char JoinTime, unsigned short JoinCycle);
unsigned char TaskJoin (unsigned short *Data);

void CallTaskOnOffAttributeChange (int8u endpoint, int8u value);
int8u TaskOnOffAttributeChange(int16u *Data);

void CallResetMCU(int16u ResetTimeCountdown);

void CallMcTimmerCalibrateTask(int8u inverval, int8u time);
int8u TaskMcTimmerCalibrate(int16u *Data);

void TaskRun (void);

#endif