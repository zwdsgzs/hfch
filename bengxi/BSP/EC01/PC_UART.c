#include "PC_UART.h"

extern _Para Para,ParaTemp;
unsigned char ack[100];


uint8_t PC_Handle(void)
{
	uint8_t rtn=0;
	 if(Rev_PC_OK==1)
	 {
		 TEST_Mode=1;
		
//			USART_UART_Trans(CM_USART1,Rev_PC_Data,strlen((const char*)Rev_PC_Data),3000);//输出串口1接收的内容	CM_USART3
			cJSON * pJson = cJSON_Parse((const char*)Rev_PC_Data); //解析收到的是不是JSON语句
		 	cJSON *params = cJSON_CreateObject();
		 /*--解析4G上报需要的参数，并写入flash--*/
			if(pJson)
			 {
 
				 cJSON *pSub  = cJSON_GetObjectItem(pJson, "cmd");
				/* -------------------------MQTT--------------------*/
				if(strncmp(pSub->valuestring,"set_mqtt",strlen("set_mqtt"))==0)	 
				{
					ParaRead();
					uint16_t setFlag = Para.ADD_O2_CO;//存储flash中Add_O2_CO的标志位
					/*-------------------------mqttBroker--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "mqttBroker");
					if(pSub!=NULL)
					{
						strcpy(Para.mqttBroker,pSub->valuestring);
						SETBIT(Para.SetFlag,BIT4);
					}
				/*--------------------------ClientID-------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "ClientID");
					if(pSub!=NULL)
					{
						strcpy(Para.mqttClientID,pSub->valuestring);
						SETBIT(Para.SetFlag,BIT0);
					}
					/*--------------------------ProductKey-------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "ProductKey");
					if(pSub!=NULL)
					{
						strcpy(Para.productKey,pSub->valuestring);
						SETBIT(Para.SetFlag,BIT1);
//						strcpy(Para.mqttClientID,pSub->valuestring);
//						SETBIT(Para.SetFlag,BIT0);
					}			  	  		  

					/*--------------------------DeviceName-------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "DeviceName");
					if(pSub!=NULL)
					{	
						strcpy(Para.deviceName,pSub->valuestring);
						SETBIT(Para.SetFlag,BIT2);
						
					}			  	  		  
				
					/*--------------------------DeviceSecret-------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "DeviceSecret");
					if(pSub!=NULL)
					{									
						strcpy(Para.deviceSecret,pSub->valuestring);
						SETBIT(Para.SetFlag,BIT3);
					}			  	  		  
					/*--------------------------updateInterval-------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "updateInterval");
					if(pSub!=NULL)
					{									
						Para.updateIntervalmins=pSub->valueint;
						SETBIT(Para.SetFlag,BIT5);			  	  		  
					}	
					Para.ADD_O2_CO=setFlag;//重新写入flash中Add_O2_CO的标志位

					ParaWrite(); //写入flash
					
					PC_set_mqtt();
				}
				 
				/* -------------------------MQTT--------------------*/
				if(strncmp(pSub->valuestring,"set_general",strlen("set_general"))==0)	 
				{
					/*-------------------------gps_report_Flag--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "gps_Flag");
					if(pSub!=NULL)
					{
						gps_report_Flag=pSub->valueint;

					}			  	  		  

					/*-------------------------Add_O2_CO--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "O2_CO");
					if(pSub!=NULL)
					{
						Para.ADD_O2_CO = pSub->valueint;
					}

					/*-------------------------CH4_MAX_thresholds--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "CH4");
					if(pSub!=NULL)
					{
						CH4_MAX = pSub->valuedouble;		  	  		  
					}

					/*-------------------------H2S_MAX_thresholds--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "H2S");
					if(pSub!=NULL)
					{
						H2S_MAX = pSub->valuedouble;				  
					}

					/*-------------------------CO_MAX_thresholds--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "CO");
					if(pSub!=NULL)
					{
						CO_MAX = pSub->valuedouble;					  
					}

					/*-------------------------O2_MAX_thresholds--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "O2");
					if(pSub!=NULL)
					{
						O2_MAX = pSub->valuedouble;				  
					}

					/*-------------------------NH3_MAX_thresholds--------------------*/
					pSub  = cJSON_GetObjectItem(pJson, "NH3");
					if(pSub!=NULL)
					{
						NH3_MAX = pSub->valuedouble;
					}
					ParaWrite(); //写入flash
					PC_set_general();		//显示回码		
				}
				/* -------------------------verson--------------------*/
				if(strncmp(pSub->valuestring,"verson",strlen("verson"))==0)	 
				{
					PC_GET_verson();
				}
				
				/* -------------------------serson--------------------*/
				if(strncmp(pSub->valuestring,"p1",strlen("p1"))==0)	 
				{
					Serson_Clean();
					//传感器数据获取
					TEST_MODE_GET_SERSON();
					PC_GET_P1();//回传上位机参数
				}
				
				/* -------------------------MQTT--------------------*/
				if(strncmp(pSub->valuestring,"message1",strlen("message1"))==0)	 
				{
					
					PC_GET_message1();//回传参数
				}
				if(strncmp(pSub->valuestring,"message2",strlen("message2"))==0)	 
				{
					
					PC_GET_message2();//回传参数
				}
				
				cJSON_Delete(pJson); 
				cJSON_Delete(params); 
				Main_Counter=30000;
			}
			Rev_PC_OK=0;Rev_PC_N=0;	
			rtn=1;				
			memset(Rev_PC_Data,0,sizeof(Rev_PC_Data)); 						
	 }
	 return rtn;
}


void PC_set_mqtt(void)
{
	cJSON *params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "cmd", "ack");
	cJSON_AddStringToObject(params, "info", "set_mqtt OK");
	char *json_str = cJSON_PrintUnformatted(params);
	USART_UART_Trans(CM_USART1,json_str,strlen((const char*)json_str),30000);//输出回码
	free(json_str);
	cJSON_Delete(params);
}

void PC_set_general(void)
{
	cJSON *params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "cmd", "ack");
	cJSON_AddStringToObject(params, "info", "set_general OK");	
	char *json_str = cJSON_PrintUnformatted(params);
	USART_UART_Trans(CM_USART1,json_str,strlen((const char*)json_str),30000);//输出回码
	free(json_str);	
	cJSON_Delete(params);
}



/*-------------查询版本号返回指令------------------*/
void PC_GET_verson(void)
{
	cJSON *params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "cmd", "ack verson");
	cJSON_AddNumberToObject(params, "software_version", software_version);	
	cJSON_AddStringToObject(params, "hardware_version", hardware_version);
	char *json_str = cJSON_PrintUnformatted(params);
	USART_UART_Trans(CM_USART1,json_str,strlen((const char*)json_str),30000);//输出回码
	free(json_str);
	cJSON_Delete(params);
}

/*-------------------------
*****设备基础参数读取
*****参数：
{
  "cmd":"ack message",
  "ProductKey":"k0bbfsp6Pgv",
  "DeviceName":"SR0001",
  "DeviceSecret":"233c66ffcf124f4c776359ccf2465869",
  "mqttBroker":"iot-06z00i1mo6drl50.mqtt.iothub.aliyuncs.com",
  "iccid":"8986045104220031633",
  "imei":"863155050295706",
  "device_electricity":3.6,
  "next_time":30,
  "gps_Flag":0,
  "O2_CO":1,
  "CH4":10.0,
  "H2S":10.0,
  "O2":10.0,
  "CO":10.0,
  "NH3":10.0
  
}

--------------------*/
void PC_GET_message1(void)
{

		/*开机获取SN*/
	Start_get_SN();
	
	cJSON *params = cJSON_CreateObject();

	cJSON_AddStringToObject(params, "cmd", "ack message1");
	cJSON_AddStringToObject(params, "ProductKey", (const char*)Para.productKey);//产品key
	cJSON_AddStringToObject(params, "DeviceName", (const char*)Para.deviceName);//设备名称
	cJSON_AddStringToObject(params, "DeviceSecret", (const char*)Para.deviceSecret);//设备密钥
	cJSON_AddStringToObject(params, "mqttBroker", (const char*)Para.mqttBroker);//mqtt连接参数
	cJSON_AddStringToObject(params, "iccid", (const char*)mqttClientID);//卡号
	cJSON_AddStringToObject(params, "imei", (const char*)mqttClientSN);//SN

	 char *json_str = cJSON_PrintUnformatted(params);
 
	USART_UART_Trans(CM_USART1,json_str,strlen((const char*)json_str),30000);//输出回码
 
	free(json_str);
  	cJSON_Delete(params);
}

void  PC_GET_message2(void)
{
		cJSON *params1 = cJSON_CreateObject();	
	cJSON_AddStringToObject(params1, "cmd", "ack message2");
	cJSON_AddNumberToObject(params1, "electricity", Get_Adc(get_battery));//电池电压
	cJSON_AddNumberToObject(params1, "next_time", Para.updateIntervalmins);//下次上报时间
	cJSON_AddNumberToObject(params1, "gps_Flag", gps_report_Flag);//是否上报GPS
	cJSON_AddNumberToObject(params1, "O2_CO", Para.ADD_O2_CO);//区分标准版和高配
	cJSON_AddNumberToObject(params1, "CH4", CH4_MAX);//甲烷阈值
	cJSON_AddNumberToObject(params1, "H2S", H2S_MAX);//H2S阈值
	cJSON_AddNumberToObject(params1, "O2", O2_MAX);//O2阈值
	cJSON_AddNumberToObject(params1, "CO", CO_MAX);//CO阈值
	cJSON_AddNumberToObject(params1, "NH3", NH3_MAX);//NH3阈值
	  char *json_str1 = cJSON_PrintUnformatted(params1);
	USART_UART_Trans(CM_USART1,json_str1,strlen((const char*)json_str1),30000);//输出回码
	free(json_str1);
		cJSON_Delete(params1);
}
/*-------------------------传感器数据读取--------------------*/
void  PC_GET_P1(void)
{
	
	cJSON *params = cJSON_CreateObject();
	cJSON_AddStringToObject(params, "cmd", "ack p1");
	cJSON_AddNumberToObject(params, "c_ch4", Get_CH4_percent());//甲烷
	cJSON_AddNumberToObject(params, "d_h2s", Get_H2S());//H2S
	cJSON_AddNumberToObject(params, "pressure", Get_CH4_Kpa());//气压
	cJSON_AddNumberToObject(params, "temp", Get_CH4_Temp());//温度
	cJSON_AddNumberToObject(params, "o2", Get_O2());//氧气
	cJSON_AddNumberToObject(params, "d_co", Get_CO());//一氧化碳
	cJSON_AddNumberToObject(params, "nh3", 0.00);//氨气
	cJSON_AddNumberToObject(params, "y_distance", Get_Radar_Air_Distance());//空高，距离水面的距离

	cJSON_AddNumberToObject(params, "longitude", Get_longitude());//经度
	cJSON_AddNumberToObject(params, "latitude", Get_latitude());//纬度
	 char *json_str = cJSON_PrintUnformatted(params);
	USART_UART_Trans(CM_USART1,json_str,strlen((const char*)json_str),30000);//输出回码
	free(json_str);
	cJSON_Delete(params);
	
}

void TEST_MODE_GET_SERSON(void)
{
	V5_SET;	
    MOTOR_POWER_SET;
    for(uint8_t i = 0;i<30;i++)
	{
		DDL_DelayMS(1000);
	}
    MOTOR_POWER_RESET;//气泵开关开启

	CH4_POWER_SET;		//3个传感器
	H2S_RADAR_POWER_SET;
    DDL_DelayMS(2000);
    
	if(Para.ADD_O2_CO==1)
	{
		o2_co_nh3_POWER_SET;
		UART_change_GPS_O2_H2S_CO(change_O2);
	}
	DDL_DelayMS(1000);
	//雷达测距轮寻发送查看空高
	for (int n=0;n<10;n++)
	{
//				printf("雷达测距");
		XKD_LD8010_Poll();
		DDL_DelayMS(1000);
	}
    	
//			printf("CO传感器");
	if(Para.ADD_O2_CO==1)
	{	
		UART_change_GPS_O2_H2S_CO(change_CO);
	}
    for(uint8_t i = 0;i<15;i++)
    {
        DDL_DelayMS(1000);
    }

	o2_co_nh3_POWER_RESET;
	UART_change_GPS_O2_H2S_CO(change_GPS);

	if(gps_report_Flag>0)
	{
		fun_gps_report();
	}
	H2S_RADAR_POWER_RESET;
	CH4_POWER_RESET;		//3个传感器

}