



//-----------------------------Khai bao thu vien---------------------//


#include "app/framework/include/af.h"
#include "task.h"
#include "ButtonExe.h"
#include "halDelay.h"
#include "UART_task.h"

//-----------------------------Khai bao bien-------------------------//


int16u TaskData[64];
extern int8u HcDefaultEP;

//------------------------------Struct------------------------------//

typedef struct {
	int16u ID;
	int16u Length;
	int16u Inverval;
	int16u MaxTime;
	int16u TimeCnt;
	int16u BeforeTimer;
}TaskMcTimmerCalibrateData;
typedef struct {
	int16u ID;
	int16u Length;
	int16u MaxDimLevel;
	int16u StepTime;
	int16u BeforeDimLevel;
	int16u BeforeTimer;
}TaskUartSendDimLevelData;
typedef struct {
    int16u BlinkLedID;
    int16u Length;
    int16u DelayTime;
    int16u BlinkCnt;
    int16u MaxCnt;
    int16u BeforeTimer;
} TaskBlinkLedData;
typedef struct {
	int16u ID;
	int16u Length;
	int16u TimeCountDown;
	int16u BeforeTimer;
}TaskResetMCUData;

typedef struct {
    int16u ButtonDetectID;
    int16u Length;
    int16u StateBefore;
    int16u PressCnt;
    int16u PressOnTime;
	int16u ComboPressCnt;

}TaskButtonDetectData;

typedef struct {
    int16u ID;
    int16u Length;
    int16u TimeMax;
	int16u TimeCnt;
    int16u Cycle;
    int16u BeforeTimer;
}TaskJoinData;

typedef struct{
	int16u ID;
	int16u Length;
	int16u Endpoint;
	int16u Value;

}TaskOnOffAttributeChangeData;


//------------------------------Xu li chuong trinh-------------------//





//------------------------------Common Utility-----------------------//


//------------------------------Task Utility-------------------------//
unsigned char GetLastTask(void){
    unsigned char Head = 0;
    unsigned char Length = 0;
    while(TaskData[Head] != 0){
        if(TaskData[Head] != 0){
            Length = TaskData[Head+1]+2;
            Head = Head+Length;
        }
    }
    return Head;
}
void TaskDelete (unsigned char head){
    unsigned char TaskDataLength = 0;
    TaskDataLength = TaskData[head+1]+2;
    MEMCOPY(&TaskData[head], &TaskData[head + TaskDataLength], (TASKDATASIZE - head - TaskDataLength)*2);
}
unsigned char FindTaskID(unsigned char TaskID){
    unsigned char Head;
    unsigned char CurrentTaskID;

    Head = 0;
    CurrentTaskID = TaskData[0];
    while(CurrentTaskID !=0){

        if(TaskID == CurrentTaskID){
            return Head;
        }
        else{
             Head = Head + TaskData[Head+1]+2;
             CurrentTaskID = TaskData[Head];
        }
    }
    return TaskOK;
}
int8u ClearTask(int8u taskID){
	int8u taskPos;
	taskPos = FindTaskID(taskID);
	if(taskPos != TaskOK){
		 TaskDelete(taskPos);
		 return TaskClearOK;
	}
	else{
		return TaskIdNotFound;
	}

}

//-----------------------------************--------------------------//


//-----------------------------Send and Receiver Utility-------------//

void SendLevelControlReadAttribute(int8u Endpoint, int8u Value){
	EmberOnOffAttributeResponse OnOffLevelAttributeResponse;

	OnOffLevelAttributeResponse.AttributeID = 0x0000;  // Current Level
	OnOffLevelAttributeResponse.Status = 0x00;   // success
	OnOffLevelAttributeResponse.DataType = 0x20; //int8u
	OnOffLevelAttributeResponse.Value = Value;

	emberAfFillCommandGlobalServerToClientReadAttributesResponse(ZCL_LEVEL_CONTROL_CLUSTER_ID,&OnOffLevelAttributeResponse, EmberOnOffAttributeResponseLength);
	emberAfSetCommandEndpoints(Endpoint,HcDefaultEP);
	emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);

}

void SendOnOffControlReadAttribute(int8u Endpoint, int8u Value){
	EmberOnOffAttributeResponse OnOffAttributeResponse;
	OnOffAttributeResponse.AttributeID = 0x0000;  // on-off
	OnOffAttributeResponse.Status = 0x00;   // success
	OnOffAttributeResponse.DataType = 0x10; //bool
	OnOffAttributeResponse.Value = Value;

	emberAfFillCommandGlobalServerToClientReadAttributesResponse(0x0006,&OnOffAttributeResponse, EmberOnOffAttributeResponseLength);
	emberAfSetCommandEndpoints(Endpoint,HcDefaultEP);
	emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);

}


int8u SendViaBindingTable (int8u sourceEndpoint, int8u Value,int8u CommandType){
/*
	int8u i;
	EmberBindingTableEntry result;
	EmberStatus bindStatus;
    EmberStatus sendStatus;
	int16u NormalTransitionTime = 0xFFFF;


	for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
		bindStatus = emberGetBinding(i, &result);
		if (bindStatus == EMBER_SUCCESS) {
			if (result.type > EMBER_MULTICAST_BINDING) {
				result.type = 4;  // last entry in the string list above
			}
			if (result.type != EMBER_UNUSED_BINDING) {
				if(result.local == sourceEndpoint){
					if(CommandType == OnOffType)  {
						if(Value == 1){
							emberAfFillCommandOnOffClusterOn();
						}
						else{
							emberAfFillCommandOnOffClusterOff();
						}
					}
					else if(CommandType == LevelType)  {
					    emberAfFillCommandLevelControlClusterMoveToLevel(Value, NormalTransitionTime);
					}
					else if(CommandType == LevelWithOnOffType){
					    emberAfFillCommandLevelControlClusterMoveToLevelWithOnOff(Value,NormalTransitionTime);
					}
					sendStatus = emberAfSendCommandUnicast(EMBER_OUTGOING_VIA_BINDING,i); // Send Bind
					if(sendStatus != EMBER_SUCCESS){
						return sendStatus;
					}
				}
			}
		}
	}
*/
    return EMBER_SUCCESS;

}


//-----------------------------************--------------------------//

//------------------------------McTimmerCalibrateTask----------------------//
void CallMcTimmerCalibrateTask(int8u inverval, int8u time){
	int8u TaskDulicateStatus;
	TaskDulicateStatus = FindTaskID(TaMcTimerCalibrate);
	if(TaskDulicateStatus == TaskOK){
		TaskMcTimmerCalibrateData  McTimmerCalibrateData;
		McTimmerCalibrateData.ID = TaMcTimerCalibrate;
		McTimmerCalibrateData.Length = 4;
		McTimmerCalibrateData.Inverval = inverval;
		McTimmerCalibrateData.MaxTime = time;
		McTimmerCalibrateData.TimeCnt = 0;
		McTimmerCalibrateData.BeforeTimer =  (int16u)halCommonGetInt32uMillisecondTick();

		MEMCOPY(&TaskData[GetLastTask()],&McTimmerCalibrateData, sizeof(TaskMcTimmerCalibrateData));
	}
}
int8u TaskMcTimmerCalibrate(int16u *Data){
	TaskMcTimmerCalibrateData  McTimmerCalibrateData;

	MEMCOPY(&McTimmerCalibrateData,Data, sizeof(TaskMcTimmerCalibrateData));

    if(CommonGetDurationTime(McTimmerCalibrateData.BeforeTimer) > McTimmerCalibrateData.MaxTime){
		UartSendMcTimerCalibrate((int8u)McTimmerCalibrateData.Inverval,
								 (int8u)McTimmerCalibrateData.MaxTime,
								 (int8u)McTimmerCalibrateData.TimeCnt);
       	McTimmerCalibrateData.TimeCnt++;
        McTimmerCalibrateData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();
    }

    if(McTimmerCalibrateData.TimeCnt > McTimmerCalibrateData.TimeCnt){
        return  CompleteTask;
	}else{
		MEMCOPY(Data, &McTimmerCalibrateData, sizeof(TaskMcTimmerCalibrateData));
		return ContinuteTask;
	}
}


//------------------------------BlinkLedTask-------------------------//
void CallBlinkLedTask (int8u n, int16u delayTime, int16u maxCnt){
    int8u TaskDulicateStatus;
    TaskDulicateStatus = FindTaskID(n);
    if(TaskDulicateStatus == TaskOK){
        TaskBlinkLedData BlinkData;
        BlinkData.BlinkLedID = n;
        BlinkData.Length = 4;
        BlinkData.DelayTime = delayTime;
        BlinkData.MaxCnt = maxCnt;
        BlinkData.BlinkCnt = 0;
        BlinkData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();

        MEMCOPY(&TaskData[GetLastTask()],&BlinkData, sizeof(TaskBlinkLedData));
    }
    else{
    }
}
void ClearAllBlinkTask(void){
	ClearTask(TaBlinkLed1);
	ClearTask(TaBlinkLed2);
	ClearTask(TaBlinkLed3);
	ClearTask(TaBlinkLed4);
	ClearTask(TaBlinkLedAll);

}
unsigned char TaskBlinkLed(int16u *Data){
	TaskBlinkLedData BlinkLedData;
	int8u Result;

	MEMCOPY(&BlinkLedData,Data, sizeof(TaskBlinkLedData));

    if(CommonGetDurationTime(BlinkLedData.BeforeTimer) > BlinkLedData.DelayTime){
		switch (BlinkLedData.BlinkLedID){
			case TaBlinkLed1:
				UartSendToggleLed(1);
			break;
			case TaBlinkLed2:
				UartSendToggleLed(2);
			break;
			case TaBlinkLed3:
				UartSendToggleLed(4);
			break;
			case TaBlinkLed4:
				UartSendToggleLed(8);
			break;
			case TaBlinkLedAll:
				UartSendToggleLed(0xF);
			break;
			default:
			break;
		}
       	BlinkLedData.BlinkCnt++;
        BlinkLedData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();
    }

    if(BlinkLedData.BlinkCnt > BlinkLedData.MaxCnt){
        Result =  CompleteTask;
	}else{
		MEMCOPY(Data, &BlinkLedData, sizeof(TaskBlinkLedData));
		Result =  ContinuteTask;
	}
	return Result;
}

//-------------------**----------********---------**-------------------//


//-----------------------------CallUartSendDimLevel------------------------------//
void CallUartSendDimLevel(int8u maxLevel, int16u duaraionTime){
	TaskUartSendDimLevelData UartSendDimLevelData;

	int8u currentLevel = emberAfReadServerAttribute(1, ZCL_LEVEL_CONTROL_CLUSTER_ID,ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
				&currentLevel, sizeof(currentLevel));
    UartSendDimLevelData.ID = TaUartSendDimLevel;
	UartSendDimLevelData.Length = 4;
	UartSendDimLevelData.MaxDimLevel = maxLevel;
	((maxLevel - currentLevel)>0)?
		(UartSendDimLevelData.StepTime = duaraionTime/(maxLevel - currentLevel)):
	  	(UartSendDimLevelData.StepTime = duaraionTime/(currentLevel - maxLevel));
	UartSendDimLevelData.BeforeDimLevel = currentLevel;
	UartSendDimLevelData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();

	MEMCOPY(&TaskData[GetLastTask()],&UartSendDimLevelData, sizeof(TaskUartSendDimLevelData));
}
int8u TaskUartSendDimLevel(int16u *Data){
	TaskUartSendDimLevelData UartSendDimLevelData;

	MEMCOPY(&UartSendDimLevelData,Data, sizeof(TaskUartSendDimLevelData));


	return CompleteTask;
}

//------------------------------ScanButtonTask-------------------------//

void CallButtonDetectTask(void){
    TaskButtonDetectData ButtonDetectData;

    ButtonDetectData.ButtonDetectID = TaButtonDetect;
    ButtonDetectData.Length = 4;
    ButtonDetectData.StateBefore = ((BUTTON0_IN >> 4) & 0x01);
    ButtonDetectData.PressCnt = 0;
    ButtonDetectData.PressOnTime = (int16u)halCommonGetInt32uMillisecondTick();
	ButtonDetectData.ComboPressCnt = 0;

    MEMCOPY(&TaskData[GetLastTask()],&ButtonDetectData, sizeof(TaskButtonDetectData));
}
unsigned char TaskButtonDetect (int16u *Data){
	unsigned int ButtonStateCurrent;
	TaskButtonDetectData ButtonDetectData;
	MEMCOPY(&ButtonDetectData, Data, sizeof(TaskButtonDetectData));

    ButtonStateCurrent =  ((BUTTON0_IN >> 4) & 0x01);

    if((ButtonStateCurrent == 0) && (ButtonDetectData.StateBefore ==1)){
        if(ButtonDetectData.PressCnt == 0){
            //--------------Check Combo button-------------//
			if((CommonGetDurationTime(ButtonDetectData.PressOnTime) < 500) &&
			   				(CommonGetDurationTime (ButtonDetectData.PressOnTime) > 100)){
				ButtonDetectData.ComboPressCnt++;
			}
			else{
				ButtonDetectData.ComboPressCnt = 0;
			}
			//--------------End Check----------------------//
            ButtonDetectData.PressOnTime = (int16u)halCommonGetInt32uMillisecondTick();

            ButtonDetectData.PressCnt += 1;
        }
    }
    else if((ButtonStateCurrent == 0) && (ButtonDetectData.StateBefore == 0)){
       if(ButtonDetectData.PressCnt == 1){
            if(CommonGetDurationTime(ButtonDetectData.PressOnTime) > 100){

				beButtonPress(1);

                ButtonDetectData.PressCnt = 0;
            }
        }
    }
	else if((ButtonStateCurrent == 0) && (ButtonDetectData.StateBefore == 1)){

	}
	else if((ButtonStateCurrent == 1) && (ButtonDetectData.StateBefore == 1)){
		//-----------------ComboButtonExe------------------//
		if(CommonGetDurationTime (ButtonDetectData.PressOnTime) > 1500){
			if(ButtonDetectData.ComboPressCnt == 1){
//					BindDevice
				beButtonPress2Time(1);

				ButtonDetectData.ComboPressCnt = 0;
			}
			else if (ButtonDetectData.ComboPressCnt == 2){
//					JoinNwk
				beButtonPress3Time(1);

				ButtonDetectData.ComboPressCnt = 0;
			}
		}
		//-----------------ComboButtonExe------------------//
	}
    ButtonDetectData.StateBefore = ((BUTTON0_IN >> 4) & 0x01);
    MEMCOPY(Data,&ButtonDetectData, sizeof(TaskButtonDetectData));
    return ContinuteTask;
}

//-----------------------------***************--------------------------//


//-----------------------------FindAndJoinTask--------------------------//
void CallJoinTask(int8u JoinTime, int16u JoinCycle){
	// if Jointime = 0xFF: join forever, MinJoinCycle is 500ms, for safe, use 5000ms to join again

	TaskJoinData JoinData;

	JoinData.ID = TaJoin;
	JoinData.Length  = 	4;
	JoinData.TimeMax =  JoinTime;
	JoinData.TimeCnt =  0;
	JoinData.Cycle = JoinCycle;
	JoinData.BeforeTimer =  (int16u)halCommonGetInt32uMillisecondTick();
//		Clear blink led if out of time out---------------------//
		ClearAllBlinkTask();
//		alway blink all led to display joining task//
		CallBlinkLedTask(TaBlinkLedAll,10000,0xFF);
//-------------------------------------------------//

	MEMCOPY(&TaskData[GetLastTask()],&JoinData, sizeof(TaskJoinData));

}
unsigned char TaskJoin (int16u *Data){
    EmberNetworkStatus NetworkStatus;
    TaskJoinData JoinData;


	NetworkStatus = emberAfNetworkState();
	MEMCOPY(&JoinData,Data, sizeof(TaskJoinData));

	if( NetworkStatus == EMBER_NO_NETWORK ){
		if((JoinData.TimeMax == 0xFF) || (JoinData.TimeCnt < JoinData.TimeMax )){
			if((CommonGetDurationTime (JoinData.BeforeTimer)) > JoinData.Cycle){
				emberAfStartSearchForJoinableNetwork();
				JoinData.TimeCnt ++;
				JoinData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();
			}
		}
		else{
			//Clear blink led if out of time out---------------------//
			ClearAllBlinkTask();
			//if the first join to network, device must be read network infomation
			GetHcActiveEndPoint();
	   	    return CompleteTask;
	   	}
	}
	else if( NetworkStatus == EMBER_JOINED_NETWORK){
			//Clear blink Led if device join a nwk
			GetHcActiveEndPoint();
			ClearAllBlinkTask();
			return CompleteTask;

	}


    MEMCOPY(Data,&JoinData,(sizeof(TaskJoinData)));
	return ContinuteTask;

}
//-----------------------------************--------------------------//


//-----------------------------ResetMCU------------------------------//
void CallResetMCU(int16u ResetTimeCountdown){
	TaskResetMCUData ResetMCUData;
	ResetMCUData.ID = TaResetMCU;
	ResetMCUData.Length = 2;
	ResetMCUData.TimeCountDown = ResetTimeCountdown;
	ResetMCUData.BeforeTimer = (int16u)halCommonGetInt32uMillisecondTick();

	MEMCOPY(&TaskData[GetLastTask()],&ResetMCUData, sizeof(TaskResetMCUData));
}
int8u TaskResetMCU(int16u *Data){
	TaskResetMCUData ResetMCUData;


	MEMCOPY(&ResetMCUData,Data, sizeof(TaskResetMCUData));

    if(CommonGetDurationTime(ResetMCUData.BeforeTimer) > ResetMCUData.TimeCountDown){
		halReboot();
		return CompleteTask;
	}
	else{
		return ContinuteTask;
	}
}



//-----------------------------************--------------------------//




//----------------------On-Off Attribute Change----------------------//




//-----------------------------************--------------------------//
void CallTaskOnOffAttributeChange (int8u endpoint, int8u value){


		TaskOnOffAttributeChangeData OnOffAttributeChangeData;
		OnOffAttributeChangeData.ID = TaOnOffAttributeChange;
		OnOffAttributeChangeData.Length = 2;
		OnOffAttributeChangeData.Endpoint = endpoint;
		OnOffAttributeChangeData.Value = value;
		MEMCOPY(&TaskData[GetLastTask()],&OnOffAttributeChangeData, sizeof(TaskOnOffAttributeChangeData));

}

int8u TaskOnOffAttributeChange(int16u *Data){

		return CompleteTask;
}

//-----------------------------************--------------------------//


//-----------------------------TaskRun-------------------------------//
void TaskRun (void){

    unsigned char TaskID;
    unsigned char Head;
    unsigned char TaskStatus;

    Head = 0;
    TaskID = TaskData[Head];
    while (TaskID !=0){
        switch (TaskID){
            case TaBlinkLed1:
                TaskStatus = TaskBlinkLed(&TaskData[Head]);
                break;
            case TaBlinkLed2:
                TaskStatus = TaskBlinkLed(&TaskData[Head]);
                break;
            case TaBlinkLed3:
                TaskStatus = TaskBlinkLed(&TaskData[Head]);
                break;
			case TaBlinkLed4:
                TaskStatus = TaskBlinkLed(&TaskData[Head]);
                break;
            case TaBlinkLedAll:
                TaskStatus = TaskBlinkLed(&TaskData[Head]);
                break;
            case TaButtonDetect:
                TaskStatus = TaskButtonDetect(&TaskData[Head]);
				break;
			case TaJoin:
				TaskStatus = TaskJoin(&TaskData[Head]);
				break;
			case TaOnOffAttributeChange:
				TaskStatus = TaskOnOffAttributeChange(&TaskData[Head]);
				break;
			case TaResetMCU:
				TaskStatus = TaskResetMCU(&TaskData[Head]);
				break;
            default:
                break;
        }
        if(TaskStatus != CompleteTask){
            Head = Head + TaskData[Head + 1]+2;
            TaskID = TaskData[Head];
        }
        else{
            TaskDelete(Head);
            Head = 0;
            TaskID = TaskData[0];
        }
    }
}


//-----------------------------************----------------------------//


//-----------------------------***END-FILE***--------------------------//





