//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "task.h"
#include "ButtonExe.h"
#include "UART_task.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"


int8u StartUpFlag = 0;
int8u HcDefaultEP = 0x01;

//------------------Khai bao bien--------------------//


//-----------------Xu li ham con----------------------//


 //---------------------Call Back Funtion------------------------------//






//-------------------On/off Cluster Server Attribute Changed------------//

/** @brief On/off Cluster Server Attribute Changed
 *
 * Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterServerAttributeChangedCallback(int8u endpoint,
								EmberAfAttributeId attributeId) {

}
//-----------------End On/off Cluster Server Attribute Changed-----------------//

/** @brief Basic Cluster Reset To Factory Defaults
 *
 *
 */
boolean emberAfBasicClusterResetToFactoryDefaultsCallback(void)
{
  return FALSE;
}
/** @brief Pre ZDO Message Received
 *
 * This function passes the application an incoming ZDO message and gives the
 * appictation the opportunity to handle it. By default, this callback returns
 * FALSE indicating that the incoming ZDO message has not been handled and
 * should be handled by the Application Framework.
 *
 * @param emberNodeId   Ver.: always
 * @param apsFrame   Ver.: always
 * @param message   Ver.: always
 * @param length   Ver.: always
 */
boolean emberAfPreZDOMessageReceivedCallback(EmberNodeId emberNodeId,
                                             EmberApsFrame* apsFrame,
                                             int8u* message,
                                             int16u length)
{
	if (apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE) {

   		int8u NumberOfEndPoint;
		NumberOfEndPoint = message[4];
		int8u i;
		for(i=0;i<NumberOfEndPoint;i++){
			emberSimpleDescriptorRequest(0x0000,
                                         message[5+i],
                                         EMBER_AF_DEFAULT_APS_OPTIONS | 0x0040);
		}

		return TRUE;
	}
	else if(apsFrame->clusterId == SIMPLE_DESCRIPTOR_RESPONSE){
		int8u HcEndPoint;
		int16u HcApplicationProfileId;
		int16u HcApplicationDeviceId;
		HcEndPoint = message[5];
		HcApplicationProfileId = (message[7]<<8) | message[6];
		HcApplicationDeviceId  = (message[9]<<8) | message[8];
		if (HcApplicationProfileId == HA_PROFILE_ID){
			HcDefaultEP =   HcEndPoint;
//			emberAfFillCommandGlobalClientToServerReadAttributes(ZCL_BASIC_CLUSTER_ID,
//                                                             ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
//                                                             32);
//			emberAfSetCommandEndpoints(1,HcEndPoint);
//			emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
		}
	}
	return FALSE;

}
void GetHcActiveEndPoint(void){
	emberActiveEndpointsRequest(0x000,EMBER_AF_DEFAULT_APS_OPTIONS | 0x0040);
}


/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function.
        Note: No callback in the
 * Application Framework is associated with resource cleanup. If you are
 * implementing your application on a Unix host where resource cleanup is a
 * consideration, we expect that you will use the standard Posix system calls,
 * including the use of atexit() and handlers for signals such as SIGTERM,
 * SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal() function to
 * register your signal handler, please mind the returned value which may be an
 * Application Framework function. If the return value is non-null, please make
 * sure that you call the returned function from your handler to avoid negating
 * the resource cleanup of the Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
	CallButtonDetectTask();
	//always send and check MC ready after reset
	UartSendCheckMcReady();

}


/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 */



void emberAfMainTickCallback(void)
{
		UartScanCommand();
		UartSendCommand();
		TaskRun();

}
