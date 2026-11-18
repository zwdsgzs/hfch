#include "EC_01.h"

char From4G[600];		
unsigned char isFrom4G_OK;	
unsigned int 	From4G_N;
unsigned char CSQ=0,InitFlag=0,mqttClientSN[30],mqttClientID[50];

char AT_Buff[500] ={0};
unsigned int get_frist_Mq_flg= 0;
bool opentop = false;
bool notsigal = true;

void EC_01_Rcv_One_Char(unsigned char u16Data)
{

	static unsigned char  Data[2];
	Data[0]=Data[1];
	Data[1]=u16Data;
//	USART_WriteData(CM_USART1,u16Data);
	if(From4G_N<600)
	{
		From4G[From4G_N++]=u16Data;
	}
	if(Data[0]==0x0d&&Data[1]==0x0A)
	{
	  isFrom4G_OK=isFrom4G_OK+1;
		//USART_WriteData(CM_USART1,'=');
	}

}

unsigned  char AT_Send(char *str,char *Find,unsigned char errN)
{ 
	AT_State state = STATE_SEND_CMD;
    int retry = 0;
    
    while (retry < errN) {
        switch (state) {
            case STATE_SEND_CMD:
				printf("%s",str);
                USART_UART_Trans(CM_USART4, (void*)str, strlen(str), 30000);
                memset(From4G, 0, sizeof(From4G));
				isFrom4G_OK=0;From4G_N=0;
                state = STATE_WAIT_RESPONSE;
                break;
                
			case STATE_WAIT_RESPONSE:
				if (!start)
				{
					return 0;
				}
                DDL_DelayMS(1800);
                state = STATE_CHECK_RESPONSE;
                break;
                
            case STATE_CHECK_RESPONSE:
                if (strstr((const char*)From4G,(const char*)Find)) {
                    return 1;
                } 
				else {
                    retry++;
                    state = STATE_SEND_CMD;
                } 
				break;
			default:
				break;
        }
    }
    return 0;
 
}

unsigned  char AT_Send_2(char *str,char *Find1,char *Find2,unsigned char errN)
{ 
	
	AT_State state = STATE_SEND_CMD;
    int retry = 0;
    
    while (retry < errN) {
        switch (state) {
            case STATE_SEND_CMD:
				printf("%s",str);
                USART_UART_Trans(CM_USART4, (void*)str, strlen((const char*)str), 30000);
                memset(From4G, 0, sizeof(From4G));
				isFrom4G_OK=0;From4G_N=0;
                state = STATE_WAIT_RESPONSE;
                break;
                
			case STATE_WAIT_RESPONSE:
				if (!start)
				{
					return 0;
				}
                DDL_DelayMS(5000);
                state = STATE_CHECK_RESPONSE;
                break;
                
            case STATE_CHECK_RESPONSE:
                if (strstr((const char*)From4G,(const char*)Find1)&&strstr((const char*)From4G,(const char*)Find2)) {
//					printf("-AT2-OK\n");
                    return 1;
                } 
				else {
                    retry++;
                    state = STATE_SEND_CMD;
                } 
				break;
			default:
				break;
        }
    }
    return 0;
}

//unsigned  char AT_Send_3(unsigned char *str,unsigned char *Find1,unsigned char *Find2,unsigned char *Find3,unsigned char errN)
//{
//  unsigned  int err=0,k=0;
//
//  LAB_AT_CMD2:
//  err++; isFrom4G_OK=0;From4G_N=0;
//  memset(From4G,0,sizeof (From4G));
//
////	printf("%s",str);
//  USART_UART_Trans(CM_USART4,str,strlen((const char*)str),30000);
//  k=0;
//  while(1)
//  {
//    DDL_DelayMS(50);
//    char *p1=strstr((const char*)From4G,(const char*)Find1);
//	char *p2=strstr((const char*)From4G,(const char*)Find2);
//	char *p3=strstr((const char*)From4G,(const char*)Find3);
//
//		if((p1!=NULL && p2!=NULL)||(p1!=NULL && p3!=NULL))
//		{
////			printf("-AT3-OK\n");
//			break;
//		}
//		if(k++>500)
//		{
//		   if(err<errN){
////			   printf("-AT3-Err\n");goto LAB_AT_CMD2;
//		   }
//			 else{
////				 printf("-AT3-Fail\n");return  0;
//			 }//fail
//		 }
//  }
//  return  1;
//}

uint8_t GET_4G_ID_SN(void)
{
	unsigned char Result=0,i,j;
  
  Result=AT_Send((char *)"AT+ECICCID\r\n",(char *)"OK",8);if(Result==0){goto  Mqtt_Fail;}
  if(Result==1)
  {
	  j=0;
	  memset(mqttClientID,0,sizeof(mqttClientID));
	  for(i=10;i<32;i++)		  
	  {
		 if((From4G[i]>='0'&&From4G[i]<='9')||(From4G[i]>='A'&&From4G[i]<='Z'))
		   mqttClientID[j++]= From4G[i]; 
	  }
  }		
  
  Result=AT_Send((char *)"AT+CGSN=1\r\n",(char *)"OK",8);if(Result==0){goto  Mqtt_Fail;}
  if(Result==1)
  {
	  j=0;
		memset(mqttClientSN,0,sizeof(mqttClientSN));
	  for(i=0;i<From4G_N;i++)		  
	  {
		 if(From4G[i]>='0'&&From4G[i]<='9')
		   mqttClientSN[j++]= From4G[i]; 
	  }
	  
  }	
  return 1;
  Mqtt_Fail:
//	printf("\n---mqtt connect Fail--\n");
  AT_Send("AT+ECRST\r\n","OK",2);
  return 0;
}


unsigned char Init_4G(void)
{
  unsigned char Result=0;
	unsigned char buff[20];
	 uint8_t count = 20;
	// 准备一个缓冲区存放Token
	char token_buf[256] = {0};
	// 设置过期时间 (Unix时间戳，例如固定为 2030-01-01 00:00:00 UTC)
	long long et = 0; 
	int ret = 0;

    Result=AT_Send((char *)"AT\r\n",(char *)"OK",8);if(Result==0){goto  Mqtt_Fail;}

	GET_4G_ID_SN();
  

 notsigal = true;
 do{
	 if(count>20){
				break;}
	  Result=AT_Send((char *)"AT+CSQ\r\n",(char *)"CSQ",10);if(Result==0){goto  Mqtt_Fail;}
	  if(Result==1)
	  {
			memset(buff,0,sizeof(buff));
		  if(strstr((const char*)From4G,(const char*)"+CSQ: "))
		{
			char* index = strstr((const char*)From4G,(const char*)"+CSQ: ");//在data_rx里找到CSQ第一次出现位置并返回地址
//			char v[4]={0};
			buff[0]=*(index+6);
			buff[1]=*(index+7); 
			CSQ=atoi((const char*)buff);
		}

	  }
	  if((CSQ>1)&&(CSQ<32))
	  {
		  notsigal = false;
		  break;
	  }

  }while(count--);
	if (notsigal)
	{
		return 0;
	}
	if(CSQ == 99)
	{
	  CSQ = 0;
	}

	//断开其他连接
	Result=AT_Send("AT+ECMTDISC=0\r\n","OK",1);
	DDL_DelayMS(1000);
	Result=AT_Send("AT+ECMTCLOSE=0\r\n","OK",1);
	DDL_DelayMS(1000);

	//Para.updateIntervalSec=CSQ;
	Result=AT_Send("AT+CFUN=1\r\n","OK",20);if(Result==0){goto  Mqtt_Fail;}
	
	Result=AT_Send("AT+CEREG?\r\n","OK",20);if(Result==0){goto  Mqtt_Fail;}
//	Result=AT_Send_3("AT+CEREG?\r\n","OK","CEREG: 0,1","CEREG: 0,5",20);if(Result==0){goto  Mqtt_Fail;}
	
	Result=AT_Send("AT+CREG=1\r\n","OK",20);if(Result==0){goto  Mqtt_Fail;}
	
	// 配置为OneNET平台
	Result=AT_Send("AT+ECMTCFG=\"cloud\",0,3,1\r\n","OK",8);if(Result==0){goto  Mqtt_Fail;}
	memset(AT_Buff,0,sizeof(AT_Buff));
	// 配置OneNET的ClientID和鉴权信息
    // 对于OneNET，ClientID是产品ID，Username是设备名，Password是Token
	sprintf(AT_Buff, "AT+ECMTCFG=\"session\",0,1\r\n"); // clean session=1
    Result = AT_Send(AT_Buff, "OK", 5);if(Result==0){goto  Mqtt_Fail;}
	
	memset(AT_Buff,0,sizeof(AT_Buff));
	// 构建TCP连接
	sprintf(AT_Buff, "AT+ECMTOPEN=0,\"%s\",1883\r\n", ParaTemp.mqttBroker); // 使用6002端口
    Result = AT_Send_2(AT_Buff, "OK", "ECMTOPEN", 8);if(Result==0){goto  Mqtt_Fail;}
	
	memset(AT_Buff,0,sizeof(AT_Buff));
	// 发起MQTT连接 (ClientID, Username, Password)
	// 1. 准备参数结构体
	onenet_msg_t msg;
	strcpy(msg.produt_id, ParaTemp.productKey);      // 产品ID
	strcpy(msg.device_name, ParaTemp.deviceName);    // 设备名
	strcpy(msg.key, ParaTemp.deviceSecret);          // 设备密钥

	et = 1893456000; // 设置过期时间 (Unix时间戳，例如固定为 2030-01-01 00:00:00 UTC)
	// 4. 生成Token
	ret = onenet_creat_token_init(&msg, et, ONENET_METHOD_MD5, token_buf, sizeof(token_buf));
	if (ret > 0) {
		// 生成成功，token_buf 里就是完整的Token字符串
		// 可以拼接 AT 命令了
		sprintf(AT_Buff, "AT+ECMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",
				ParaTemp.deviceName,      // ClientID = 设备名
				ParaTemp.productKey,      // Username = 产品ID
				token_buf);               // Password = 生成的Token
		Result=AT_Send_2(AT_Buff,"OK","ECMTCONN",8);if(Result==0){goto  Mqtt_Fail;}
	} 

	if(Result ==1 )
	{
//		printf("\n---mqtt connect ok---\n");
		InitFlag=1; 
		return 1;
	}
	

  Mqtt_Fail:
	InitFlag = 0;
//	printf("\n---mqtt connect Fail--\n");
	AT_Send("AT+ECRST\r\n","OK",2);
	return 0;
} 

void Mq_Sub_shadow(void)//低功耗断连后，需重新订阅期望地址
{
	char Topic[200],Topic1[200];
	unsigned char payload[80]="{\"id\":\"12\",\"version\":\"1.0\",\"params\":[\"HeartFreq\",\"Latitude\",\"Longitude\"]}";
//	unsigned char payload[80]="{\"id\":\"12\",\"version\":\"1.0\",\"params\":\"HeartBeatInterval\"}";
	unsigned char Result;
	unsigned char uchFlashCnt = 0;

	if(get_frist_Mq_flg ==2)
	{
			memset(AT_Buff,0,sizeof(AT_Buff));
			memset(Topic1,0,sizeof(Topic1));
			sprintf(Topic1,"$sys/%s/%s/thing/property/desired/get/reply",ParaTemp.productKey,ParaTemp.deviceName);
			sprintf(AT_Buff,"AT+ECMTSUB=0,1,\"%s\",1\r\n",Topic1);
			Result=AT_Send_2(AT_Buff,"OK","+ECMTSUB",8);
		get_frist_Mq_flg++;
	}
	memset(Topic,0,sizeof(Topic));
	memset(AT_Buff,0,sizeof(AT_Buff));
	sprintf(Topic,"$sys/%s/%s/thing/property/desired/get",ParaTemp.productKey,ParaTemp.deviceName);
	sprintf(AT_Buff,"AT+ECMTPUB=0,1,1,0,\"%s\",\"%s\"\r\n",Topic,payload);
	Result=AT_Send_2(AT_Buff,"OK","+ECMTRECV",8);
	if(Result == NULL)
	{
		return;
	}

//	char stryd[] = "{\"msg\":\"success\",\"code\":200,\"data\":{\"HeartFreq\":{\"version\":1,\"value\":2},\"Latitude\":{\"version\":1,\"value\":32.1},\"Longitude\":{\"version\":1,\"value\":118.2}},\"id\":\"123\"}";
//	char* result = extract_content(stryd);
	char* result = extract_content(From4G);//截取json字符串
	if (result != NULL) 
	{
		if(get_json_value(result)==0)
		{
			if(Para.updateIntervalmins != ParaTemp.updateIntervalmins)
			{
				Para.updateIntervalmins = ParaTemp.updateIntervalmins;

				uchFlashCnt ++;
			}
		}
		if(get_json_value1(result)==0)
		{
			if (Para.unLat != ParaTemp.unLat)
			{
				Para.unLat = ParaTemp.unLat;
				Para.dbLat = ParaTemp.dbLat;
				uchFlashCnt ++;
			}
		}
		if(get_json_value2(result)==0)
		{
			if (Para.unLon != ParaTemp.unLon)
			{
				Para.unLon = ParaTemp.unLon;
				Para.dbLon = ParaTemp.dbLon;
				uchFlashCnt ++;
			}
		}
		if (uchFlashCnt != 0)
		{
			ParaWrite();
			DDL_DelayMS(4000);
		}
		memset(From4G,0,sizeof(From4G));
		free(result);
	}
	else
	{
//		printf("sssss--------xxx\n");
	}
	
}


void Mq_Pub_P(void)
{
	char Topic[200];
	char payload[420];
	unsigned char Result=0;
	DDL_DelayMS(1000);
	if(get_frist_Mq_flg==1)
	{
		if(!opentop)
		{
			memset(AT_Buff,0,sizeof(AT_Buff));
			memset(Topic,0,sizeof(Topic));
			sprintf(Topic,"$sys/%s/%s/thing/property/post/reply",ParaTemp.productKey,ParaTemp.deviceName);
			sprintf(AT_Buff,"AT+ECMTSUB=0,1,\"%s\",1\r\n",Topic);
			Result=AT_Send_2(AT_Buff,"OK","+ECMTSUB",8);
			if(Result == NULL)
			{
				return;
			}
		}
		get_frist_Mq_flg++;
	}
	cJSON *root = cJSON_CreateObject();
	cJSON *params = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "id", "12"); //
	cJSON_AddStringToObject(root, "version", "1.0"); //

	// 每个属性都创建一个对象，里面放 value
//	cJSON *CoverStatus_obj = cJSON_CreateObject();
//	cJSON_AddBoolToObject(CoverStatus_obj, "value", stu_sensor.open);
//	cJSON_AddItemToObject(params, "CoverStatus", CoverStatus_obj);

	cJSON *CSQ_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(CSQ_obj, "value", CSQ);
	cJSON_AddItemToObject(params, "SignalQuality", CSQ_obj);

	if (stu_sensor.usCH4 > 100)
	{
		stu_sensor.usCH4 = 100;
	}
	cJSON *CH4_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(CH4_obj, "value", stu_sensor.usCH4);
	cJSON_AddItemToObject(params, "CH4", CH4_obj);

	stu_sensor.usH2S = getusData(stu_sensor.sH2S);

	if (stu_sensor.usH2S > 100)
	{
		stu_sensor.usH2S = 100;
	}
	cJSON *H2S_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(H2S_obj, "value", stu_sensor.usH2S);
	cJSON_AddItemToObject(params, "H2S", H2S_obj);

	stu_sensor.usLD = getusData(stu_sensor.sLD);
	if (stu_sensor.usLD == 0)
	{
		stu_sensor.uchLDCnt ++;
	}
	else{
		stu_sensor.uchLDCnt = 0;
	}
	cJSON *Radar_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(Radar_obj, "value", stu_sensor.usLD);
	cJSON_AddItemToObject(params, "Radar", Radar_obj);

//	cJSON *HeartFreq_obj = cJSON_CreateObject();
//	cJSON_AddNumberToObject(HeartFreq_obj, "value", stu_sensor.uchCount);
//	cJSON_AddItemToObject(params, "HeartFreq", HeartFreq_obj);


	if (stu_sensor.batter > 100)
	{
		stu_sensor.batter = 100;
	}
	cJSON *Battery_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(Battery_obj, "value", stu_sensor.batter);
	cJSON_AddItemToObject(params, "Battery", Battery_obj);

	if (Para.dbLon < 0)
	{
		Para.dbLon = 0;
	}
	if (Para.dbLon > 180)
	{
		Para.dbLon = 180;
	}
	cJSON *Longitude_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(Longitude_obj, "value", Para.dbLon);
	cJSON_AddItemToObject(params, "Longitude", Longitude_obj);

	if (Para.dbLat < 0)
	{
		Para.dbLat = 0;
	}
	if (Para.dbLat > 90)
	{
		Para.dbLat = 90;
	}
	cJSON *Latitude_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(Latitude_obj, "value", Para.dbLat);
	cJSON_AddItemToObject(params, "Latitude", Latitude_obj);

	cJSON_AddItemToObject(root, "params", params);

	char *json_str = cJSON_PrintUnformatted(root);

	memset(payload,0,sizeof(payload));
	strcat(payload,json_str);
	free(json_str);
	cJSON_Delete(root);
//	printf("CH4_percent=[%.2f]  CH4_Kpa=[%.2f]  CH4_Temp=[%.2f] H2S=[%d]  O2=[%.2f] CO=[%.2f] Radar_Air_Distance=[%d] \n", CH4_percent, CH4_Kpa, CH4_Temp, H2S, O2, CO, Radar_Air_Distance);
	memset(AT_Buff,0,sizeof(AT_Buff));
	memset(Topic,0,sizeof(Topic));
	sprintf(Topic,"$sys/%s/%s/thing/property/post",ParaTemp.productKey,ParaTemp.deviceName);
	sprintf(AT_Buff,"AT+ECMTPUB=0,1,1,0,\"%s\",\"%s\"\r\n",Topic,payload);

	if ((stu_sensor.uchLDCnt == 0)||(stu_sensor.uchLDCnt == 3))
	{
		Result = AT_Send_2(AT_Buff, "OK", "+ECMTRECV:", 8);
		if(stu_sensor.uchLDCnt == 3)
		{
			stu_sensor.uchLDCnt = 0;
		}
	}
}

void Mq_Pub_open(void)
{
	char Topic[200];
	char payload[400];
	unsigned char Result=0;
	DDL_DelayMS(1000);
	if(!opentop)
	{
		memset(AT_Buff,0,sizeof(AT_Buff));
		memset(Topic,0,sizeof(Topic));
		sprintf(Topic,"$sys/%s/%s/thing/property/post/reply",ParaTemp.productKey,ParaTemp.deviceName);
		sprintf(AT_Buff,"AT+ECMTSUB=0,1,\"%s\",1\r\n",Topic);
		Result=AT_Send_2(AT_Buff,"OK","+ECMTSUB",8);
		if(Result == NULL)
		{
			return;
		}
		opentop = true;
	}
	cJSON *root = cJSON_CreateObject();
	cJSON *params = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "id", "12"); //
	cJSON_AddStringToObject(root, "version", "1.0"); //

	// 每个属性都创建一个对象，里面放 value
	cJSON *CoverStatus_obj = cJSON_CreateObject();
	cJSON_AddBoolToObject(CoverStatus_obj, "value", stu_sensor.open);
	cJSON_AddItemToObject(params, "CoverStatus", CoverStatus_obj);

	cJSON_AddItemToObject(root, "params", params);

	char *json_str = cJSON_PrintUnformatted(root);

	memset(payload,0,sizeof(payload));
	strcat(payload,json_str);
	free(json_str);
	cJSON_Delete(root);
//	printf("CH4_percent=[%.2f]  CH4_Kpa=[%.2f]  CH4_Temp=[%.2f] H2S=[%d]  O2=[%.2f] CO=[%.2f] Radar_Air_Distance=[%d] \n", CH4_percent, CH4_Kpa, CH4_Temp, H2S, O2, CO, Radar_Air_Distance);
	memset(AT_Buff,0,sizeof(AT_Buff));
	memset(Topic,0,sizeof(Topic));
	sprintf(Topic,"$sys/%s/%s/thing/property/post",ParaTemp.productKey,ParaTemp.deviceName);
	sprintf(AT_Buff,"AT+ECMTPUB=0,1,1,0,\"%s\",\"%s\"\r\n",Topic,payload);

	
	Result = AT_Send_2(AT_Buff, "OK", "+ECMTRECV:", 8);
}


char* extract_content(const char* str) {
	// 找到第一个 '{' 的位置
	char *start = strchr(str, '{');
    if (start == NULL) {
		//printf("未找到第一个 '{'\n"); 
        return NULL;
    }

	// 找到最后一个 '}' 的位置
    char *end = strrchr(str, '}');
    if (end == NULL) {
        //printf("未找到最后一个 '}'\n");
        return NULL;
    }

	// 计算中间内容的长度
    int length = end - start + 1;

 // 截取中间的内容
    char* middle = (char*)malloc((length + 1) * sizeof(char));
    if (middle == NULL) {
        //printf("内存分配失败\n");
        return NULL;
    }
    strncpy(middle, start, length);
    middle[length] = '\0';  // 添加字符串结束符

	
    return middle;
}


//解析影子信息，上报次数
int get_json_value(const char* json_str) {
// 解析 JSON 字符串
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) {
//        printf("json parse error\n");
        return 1;
    }

    // 获取 "data" 对象
    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "HeartFreq" 对象
    cJSON* heartFreq = cJSON_GetObjectItem(data, "HeartFreq");
    if (heartFreq == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "value" 
    cJSON* fre = cJSON_GetObjectItem(heartFreq, "value");
    if (fre == NULL) {
//        printf("unfind \"payload\" object\n");
        cJSON_Delete(root);
        return 1;
    }

	cJSON_Delete(root);		
	stu_sensor.uchCount=fre->valueint;
	ParaTemp.updateIntervalmins=24*60/stu_sensor.uchCount;

	return  0;
}

//解析影子信息，纬度
int get_json_value1(const char* json_str) {
// 解析 JSON 字符串
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) {
//        printf("json parse error\n");
        return 1;
    }

    // 获取 "data" 对象
    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "HeartFreq" 对象
    cJSON* Latitude = cJSON_GetObjectItem(data, "Latitude");
    if (Latitude == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "value" 
    cJSON* lat = cJSON_GetObjectItem(Latitude, "value");
    if (lat == NULL) {
//        printf("unfind \"payload\" object\n");
        cJSON_Delete(root);
        return 1;
    }

	cJSON_Delete(root);		
	ParaTemp.dbLat = lat->valuedouble;
	ParaTemp.unLat = (unsigned int)(ParaTemp.dbLat * 10000);

	return  0;
}

//解析影子信息，经度
int get_json_value2(const char* json_str) {
// 解析 JSON 字符串
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) {
//        printf("json parse error\n");
        return 1;
    }

    // 获取 "data" 对象
    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "HeartFreq" 对象
    cJSON* Longitude = cJSON_GetObjectItem(data, "Longitude");
    if (Longitude == NULL) {
        cJSON_Delete(root);
        return 1;
    }

    // 获取 "value" 
    cJSON* lon = cJSON_GetObjectItem(Longitude, "value");
    if (lon == NULL) {
//        printf("unfind \"payload\" object\n");
        cJSON_Delete(root);
        return 1;
    }

	cJSON_Delete(root);		
	ParaTemp.dbLon = lon->valuedouble;
	ParaTemp.unLon = (unsigned int)(ParaTemp.dbLon * 10000);

	return  0;
}

unsigned char reConnect_4G(void)//低功耗断连后，需重新open+connect
{
  unsigned char Result=0;
	unsigned char buff[20];
	notsigal = true;
	// 准备一个缓冲区存放Token
	char token_buf[256] = {0};
	// 设置过期时间 (Unix时间戳，例如固定为 2030-01-01 00:00:00 UTC)
	long long et = 0; 
	int ret = 0;

	CSQ=0;
	for (int i = 0; i < 10; ++i)
	{
		Result=AT_Send((char *)"AT+CSQ\r\n",(char *)"CSQ",10);if(Result==0){goto  Mqtt_Fail;}
	  if(Result==1)
	  {
			memset(buff,0,sizeof(buff));
		  if(strstr((const char*)From4G,(const char*)"+CSQ: "))
		{
			char* index = strstr((const char*)From4G,(const char*)"+CSQ: ");//在data_rx里找到CSQ第一次出现位置并返回地址
//			char v[4]={0};
			buff[0]=*(index+6);
			buff[1]=*(index+7); 
			CSQ=atoi((const char*)buff);
		}

	  }
	  if((CSQ>1)&&(CSQ<32))
	  {
		  notsigal = false;
		  break;
	  }
	}

	if (notsigal)
	{
		return 0;
	}

	if(CSQ == 99)
	{
	  CSQ = 0;
	}

    memset(AT_Buff,0,sizeof(AT_Buff));
	// 构建TCP连接
	sprintf(AT_Buff, "AT+ECMTOPEN=0,\"%s\",1883\r\n", ParaTemp.mqttBroker); // 使用6002端口
    Result = AT_Send_2(AT_Buff, "OK", "ECMTOPEN", 3);if(Result==0){goto  Mqtt_Fail;}
	
	memset(AT_Buff,0,sizeof(AT_Buff));
	// 发起MQTT连接 (ClientID, Username, Password)
	// 1. 准备参数结构体
	onenet_msg_t msg;
	strcpy(msg.produt_id, ParaTemp.productKey);      // 产品ID
	strcpy(msg.device_name, ParaTemp.deviceName);    // 设备名
	strcpy(msg.key, ParaTemp.deviceSecret);          // 设备密钥

	et = 1893456000; //设置过期时间 (Unix时间戳，例如固定为 2030-01-01 00:00:00 UTC)
	// 4. 生成Token
	ret = onenet_creat_token_init(&msg, et, ONENET_METHOD_MD5, token_buf, sizeof(token_buf));
	if (ret > 0) {
		// 生成成功，token_buf 里就是完整的Token字符串
		// 可以拼接 AT 命令了
		sprintf(AT_Buff, "AT+ECMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",
				ParaTemp.deviceName,      // ClientID = 设备名
				ParaTemp.productKey,      // Username = 产品ID
				token_buf);               // Password = 生成的Token
		Result=AT_Send_2(AT_Buff,"OK","ECMTCONN",3);if(Result==0){goto  Mqtt_Fail;}
	} 

//	sprintf(AT_Buff, "AT+ECMTDISC=0");//断开连接

	if(Result ==1 )
	{
//		printf("\n---mqtt connect ok---\n");
		InitFlag=1; 
		return 1;
	}
	

  Mqtt_Fail:
	InitFlag = 0;
//	printf("\n---mqtt connect Fail--\n");
	AT_Send("AT+ECRST\r\n","OK",2);
	return 0;
} 

