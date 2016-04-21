#ifndef CONFIGDEVICE_H
#define CONFIGDEVICE_H

#ifdef  SWITCH_1_BUTTON
	#ifndef NUMBER_BUTTON
	#define NUMBER_BUTTON 	 1
	#define SWITCH_TYPE
	#endif
#endif

#ifdef  SWITCH_2_BUTTON
	#ifndef NUMBER_BUTTON
	#define NUMBER_BUTTON 	 2
	#define SWITCH_TYPE
	#endif
#endif

#ifdef  SWITCH_3_BUTTON
	#ifndef NUMBER_BUTTON
	#define NUMBER_BUTTON 	 3
	#define SWITCH_TYPE
	#endif
#endif

#ifdef  SWITCH_4_BUTTON
	#ifndef NUMBER_BUTTON
	#define NUMBER_BUTTON 	 4
	#define SWITCH_TYPE
	#endif
#endif

#ifdef DIMMER
	#define DIMMER_TYPE
#endif

#define Button1InClusterEndpoint 1
#define Button1OutClusterEndpoint 2
#define Button2InClusterEndpoint 3
#define Button2OutClusterEndpoint 4
#define Button3InClusterEndpoint 5
#define Button3OutClusterEndpoint 6
#define Button4InClusterEndpoint 7
#define Button4OutClusterEndpoint 8
#define DimmerInClusterEndpoint	1
#define DimmerOutClusterEndpoint 2

#define Lumivietnam   	12,'L','u','m','i',' ','V','i','e','t','n','a','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LumiRD     		8,'L','u','m','i',' ','R','&','D',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_DZ1     		6,'L','M','-','D','Z','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_DZ2      	6,'L','M','-','D','Z','2',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_SZ1       	6,'L','M','-','S','Z','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_SZ2       	6,'L','M','-','S','Z','2',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_SZ3       	6,'L','M','-','S','Z','3',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_SZ4       	6,'L','M','-','S','Z','4',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define LM_BZ1			6,'L','M','-','B','Z','1',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

#ifdef DIMMER
	#ifndef CURTAIN
			#define ManufacturerName   LumiRD
			#define ModelId	LM_DZ1
	#else
			#define ManufacturerName   LumiRD
			#define ModelId	LM_BZ1
	#endif
#endif

#ifdef SWITCH_1_BUTTON
		#define ManufacturerName   LumiRD
		#define ModelId				LM_SZ1
#endif
#ifdef SWITCH_2_BUTTON
		#define ManufacturerName   LumiRD
		#define ModelId				LM_SZ2
#endif
#ifdef SWITCH_3_BUTTON
		#define ManufacturerName   LumiRD
		#define ModelId				LM_SZ3
#endif
#ifdef SWITCH_4_BUTTON
		#define ManufacturerName   LumiRD
		#define ModelId				LM_SZ4
#endif

#define LedLightOptionEndpoint		15
//#define Zipato
//
//#ifdef Zipato
//#define HcDefaultEP	0x0C
//#endif
//#ifdef SmartThing
//#define  HcDefaultEP 0x01
//#endif
//#ifndef  HcDefaultEP
//#define	HcDefaultEP	0x01
//#endif


#endif