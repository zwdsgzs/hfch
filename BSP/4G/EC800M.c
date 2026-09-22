#include "EC800M.h"

AT_Cmd_t at_cmd;
stu4GDef stu_4G = {0};
unsigned char From4G[600] = {0};//存储4G数据

static FlowStep_t flow_step = STEP_IDLE;//流程
static char       g_pub_payload[400];

/* ============================================================
 *                      从环形缓冲区搬数据
 * ============================================================ */

void EC800M_ProcessRxData(void)
{
    uint8_t  tmp[64];
    uint16_t n;

    while (!ring_buf_is_empty(&rb_usart1))
    {
        n = ring_buf_read(&rb_usart1, tmp, sizeof(tmp));
        if (n == 0) 
		{
			break;
		}

        for (uint16_t i = 0; i < n; i++)
        {
            if (stu_4G.From4G_N < sizeof(From4G) - 1)
                From4G[stu_4G.From4G_N++] = tmp[i];

            if (stu_4G.From4G_N >= 2 &&
                From4G[stu_4G.From4G_N - 2] == 0x0D &&
                From4G[stu_4G.From4G_N - 1] == 0x0A)
            {
                stu_4G.isFrom4G_OK++;
            }
        }
    }
}

/* ============================================================
 *                      AT 状态机
 * ============================================================ */

uint8_t AT_Start(char *str, char *find1, char *find2,
                 uint8_t errN, uint32_t timeout_ms)
{
    if (at_cmd.busy)  // 如果已经在执行，拒绝新命令
	{
		return 0;
	}

	// 清空状态
    memset(&at_cmd, 0, sizeof(at_cmd));

	// 拷贝参数
    strncpy(at_cmd.cmd,   str,   sizeof(at_cmd.cmd) - 1);
    strncpy(at_cmd.find1, find1, sizeof(at_cmd.find1) - 1);

    if (find2 != NULL && find2[0] != '\0') 
	{
        strncpy(at_cmd.find2, find2, sizeof(at_cmd.find2) - 1);
        at_cmd.need_two = 1;
    }

	// 设置参数
    at_cmd.retry      = errN;
    at_cmd.timeout_ms = timeout_ms;
    at_cmd.result     = 0;
    at_cmd.busy       = 1;
    at_cmd.state      = AT_STATE_SEND;

    return 1;
}

void AT_Task(void)
{
    if (!at_cmd.busy) // 空闲状态，直接返回
	{
		return;
	}

	 // 不管什么状态，先把环形缓冲区数据搬到 From4G
    EC800M_ProcessRxData();

    switch (at_cmd.state)// 根据当前状态执行
    {
        case AT_STATE_SEND:// 发送命令
            uartSendIT(1, (void*)at_cmd.cmd, strlen(at_cmd.cmd));

			//清空本地缓冲
            memset(From4G, 0, sizeof(From4G));
            stu_4G.isFrom4G_OK = 0;
            stu_4G.From4G_N     = 0;
            ring_buf_clear(&rb_usart1);

			// 记录发送时刻
            at_cmd.tick_start = HAL_GetTick();
			// 进入等待状态
            at_cmd.state = AT_STATE_WAIT;
            break;
        case AT_STATE_WAIT:// 等待响应
            if ((HAL_GetTick() - at_cmd.tick_start) >= at_cmd.timeout_ms) // 超时检查
			{
                at_cmd.state = AT_STATE_CHECK;
                break;
            }

			// 提前退出：关键字出现了就不用等满超时
            if (at_cmd.need_two) 
			{
                if (strstr((const char*)From4G, at_cmd.find1) &&
                    strstr((const char*)From4G, at_cmd.find2)) 
				{
                    at_cmd.state = AT_STATE_CHECK;
                }
            } else {
                if (strstr((const char*)From4G, at_cmd.find1)) 
				{
                    at_cmd.state = AT_STATE_CHECK;
                }
            }
            break;
        case AT_STATE_CHECK:// 检查结果
        {
            uint8_t found = 0;

            if (at_cmd.need_two) // 判断关键字是否出现
			{
                if (strstr((const char*)From4G, at_cmd.find1) &&
                    strstr((const char*)From4G, at_cmd.find2)) 
				{
                    found = 1;
                }
            } else {
                if (strstr((const char*)From4G, at_cmd.find1)) 
				{
                    found = 1;
                }
            }

            if (found) // 成功
			{
                at_cmd.result = 1;
                at_cmd.busy   = 0;
                at_cmd.state  = AT_STATE_IDLE;
            } else {// 失败，看是否还有重试机会
                if (at_cmd.retry > 0) 
				{
                    at_cmd.retry--;
                    at_cmd.state = AT_STATE_SEND;// 重发
                } else {
                    at_cmd.result = 0;
                    at_cmd.busy   = 0;
                    at_cmd.state  = AT_STATE_IDLE;
                }
            }
            break;
        }

        default:
            at_cmd.state = AT_STATE_IDLE;
            at_cmd.busy  = 0;
            break;
    }
}

uint8_t AT_IsDone(uint8_t *pResult)
{
    if (at_cmd.busy)  // 还在执行
	{
		return 0;
	}
    if (pResult) // 已完成
	{
		*pResult = at_cmd.result;
	}
    return 1;
}

/* ============================================================
 *                      解析辅助
 * ============================================================ */

static void ParseICCID(void)
{
    unsigned char i, j = 0;
    memset(stu_4G.mqttClientID, 0, sizeof(stu_4G.mqttClientID));
    for (i = 0; i < stu_4G.From4G_N; i++) 
	{
        if ((From4G[i] >= '0' && From4G[i] <= '9') ||
            (From4G[i] >= 'A' && From4G[i] <= 'Z')) 
		{
            if (j < sizeof(stu_4G.mqttClientID) - 1)
			{
				stu_4G.mqttClientID[j++] = From4G[i];
			}
        }
    }
}

static void ParseIMEI(void)
{
    unsigned char i, j = 0;
    memset(stu_4G.mqttClientSN, 0, sizeof(stu_4G.mqttClientSN));
    for (i = 0; i < stu_4G.From4G_N; i++) 
	{
        if (From4G[i] >= '0' && From4G[i] <= '9') 
		{
            if (j < sizeof(stu_4G.mqttClientSN) - 1)
			{
				stu_4G.mqttClientSN[j++] = From4G[i];
			}
        }
    }
}

static void ParseCSQ(void)
{
    unsigned char buff[4] = {0};
    if (strstr((const char*)From4G, "+CSQ: ")) 
	{
        char *index = strstr((const char*)From4G, "+CSQ: ");
        buff[0] = *(index + 6);
        buff[1] = *(index + 7);
        stu_4G.CSQ = atoi((const char*)buff);
    }
}

/* ============================================================
 *                      启动流程
 * ============================================================ */

void Flow_Start(void)
{
    stu_4G.flow_csq_retry = 0;

    if (stu_4G.InitFlag == 0) 
	{
        /* 首次上电：从 AT 开始，走完整流程 */
        flow_step = STEP_AT;
    } else {
        /* 唤醒后：从 CSQ 开始，只重连 */
        flow_step = STEP_CSQ;
    }

    memset(&at_cmd, 0, sizeof(at_cmd));
}

/* ============================================================
 *                      流程状态机
 * ============================================================
 * 每个 case 只做一件事：
 *   1. 等待 AT 命令完成
 *   2. 根据结果决定下一步
 *   3. 启动下一条 AT 命令
 * ============================================================ */

void Flow_Task(void)
{
    uint8_t result;
    char    buf[512];
    char    topic[256];
    char    token_buf[256];
    onenet_msg_t msg;

    /* 空闲/完成/失败，不处理 */
    if (flow_step == STEP_IDLE ||
        flow_step == STEP_DONE ||
        flow_step == STEP_FAIL) 
	{
		return;
	}

    /* 等当前 AT 命令完成 */
    if (!AT_IsDone(&result)) 
	{
		return;
	}

    switch (flow_step)
    {
        /* ---------- 冷启动专属步骤 ---------- */
        case STEP_AT:
            if (result) 
			{ 
				flow_step = STEP_QCCID; 
				AT_Start("AT+QCCID\r\n", "OK", NULL, 8, 1800); 
			}
            else{ 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_QCCID:
            if (result) 
			{ 
				ParseICCID(); 
				flow_step = STEP_CGSN;
				AT_Start("AT+CGSN=1\r\n", "OK", NULL, 8, 1800); 
			}
            else{ 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_CGSN:
            if (result) 
			{ 
				ParseIMEI(); 
				flow_step = STEP_CSQ; 
				AT_Start("AT+CSQ\r\n", "CSQ", NULL, 10, 1800); 
			}
            else{ 
				flow_step = STEP_FAIL; 
			}
            break;

        /* ---------- 冷热启动都走 ---------- */
        case STEP_CSQ:
            if (result) 
			{
                ParseCSQ();
                if (stu_4G.CSQ > 1 && stu_4G.CSQ < 99) 
				{
                    stu_4G.notsigal = 0;
                    if (stu_4G.CSQ < 12) stu_4G.CSQ = 15;

                    if (stu_4G.InitFlag == 0) 
					{
                        /* 冷启动：继续走 */
                        flow_step = STEP_QMTDISC;
                        AT_Start("AT+QMTDISC=0\r\n", "OK", NULL, 1, 1800);
                    } else {
                        /* 热启动：跳到 QMTOPEN */
                        flow_step = STEP_QMTOPEN;
                        snprintf(buf, sizeof(buf),
                                 "AT+QMTOPEN=0,\"%s\",1883\r\n", stu_flash.mqttBroker);
                        AT_Start(buf, "OK", "QMTOPEN", 8, 15000);
                    }
                } else {
                    if (++stu_4G.flow_csq_retry > 20) 
					{ 
						flow_step = STEP_FAIL; 
					}
                    else { 
						AT_Start("AT+CSQ\r\n", "CSQ", NULL, 10, 1800); 
					}
                }
            } else { flow_step = STEP_FAIL; }
            break;

        /* ---------- 冷启动专属步骤（继续） ---------- */
        case STEP_QMTDISC:
            flow_step = STEP_QMTCLOSE;
            AT_Start("AT+QMTCLOSE=0\r\n", "OK", NULL, 1, 1800);
            break;

        case STEP_QMTCLOSE:
            flow_step = STEP_CFUN;
            AT_Start("AT+CFUN=1\r\n", "OK", NULL, 20, 15000);
            break;

        case STEP_CFUN:
            if (result) 
			{ 
				flow_step = STEP_CEREG; 
				AT_Start("AT+CEREG?\r\n", "OK", NULL, 20, 5000); 
			}
            else{ 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_CEREG:
            if (result) 
			{ 
				flow_step = STEP_CREG; 
				AT_Start("AT+CREG=1\r\n", "OK", NULL, 20, 5000); 
			}
            else{ 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_CREG:
            if (result) 
			{
                flow_step = STEP_QMTCFG_CLOUD;
                AT_Start("AT+QMTCFG=\"cloud\",0,3,1\r\n", "OK", NULL, 8, 1800);
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_QMTCFG_CLOUD:
            flow_step = STEP_QMTCFG_SESSION;
            AT_Start("AT+QMTCFG=\"session\",0,1\r\n", "OK", NULL, 5, 1800);
            break;

        case STEP_QMTCFG_SESSION:
            if (result) 
			{
                snprintf(buf, sizeof(buf),
                         "AT+QMTOPEN=0,\"%s\",1883\r\n", stu_flash.mqttBroker);
                flow_step = STEP_QMTOPEN;
                AT_Start(buf, "OK", "QMTOPEN", 8, 15000);
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        /* ---------- 冷热启动都走（继续） ---------- */
        case STEP_QMTOPEN:
            if (result) 
			{
                strcpy(msg.produt_id,   stu_flash.productKey);
                strcpy(msg.device_name, stu_flash.deviceName);
                strcpy(msg.key,         stu_flash.deviceSecret);

                if (onenet_creat_token_init(&msg, 1893456000, ONENET_METHOD_MD5,
                                            token_buf, sizeof(token_buf)) > 0) {
                    snprintf(buf, sizeof(buf),
                             "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",
                             stu_flash.deviceName, stu_flash.productKey, token_buf);
                    flow_step = STEP_QMTCONN;
                    AT_Start(buf, "OK", "QMTCONN", 8, 15000);
                } else { flow_step = STEP_FAIL; }
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_QMTCONN:
            if (result) 
			{
                stu_4G.InitFlag = 1;
                /* 连接成功，进入订阅步骤 */
                flow_step = STEP_SUB_POST_REPLY;
                snprintf(topic, sizeof(topic),
                         "$sys/%s/%s/thing/property/post/reply",
                         stu_flash.productKey, stu_flash.deviceName);
                snprintf(buf, sizeof(buf), "AT+QMTSUB=0,1,\"%s\",1\r\n", topic);
                AT_Start(buf, "OK", "+QMTSUB", 8, 5000);
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        /* ---------- 订阅步骤 ---------- */
        case STEP_SUB_POST_REPLY:
            if (result) 
			{
                flow_step = STEP_SUB_DESIRED_REPLY;
                snprintf(topic, sizeof(topic),
                         "$sys/%s/%s/thing/property/desired/get/reply",
                         stu_flash.productKey, stu_flash.deviceName);
                snprintf(buf, sizeof(buf), "AT+QMTSUB=0,1,\"%s\",1\r\n", topic);
                AT_Start(buf, "OK", "+QMTSUB", 8, 5000);
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_SUB_DESIRED_REPLY:
            if (result) 
			{
                unsigned char payload[80] =
                    "{\"id\":\"12\",\"version\":\"1.0\","
                    "\"params\":[\"HeartFreq\",\"Latitude\",\"Longitude\"]}";
                flow_step = STEP_SUB_DESIRED_GET;
                snprintf(topic, sizeof(topic),
                         "$sys/%s/%s/thing/property/desired/get",
                         stu_flash.productKey, stu_flash.deviceName);
                snprintf(buf, sizeof(buf),
                         "AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n",
                         topic, (int)strlen((char*)payload));
                AT_Start(buf, "OK", "+QMTRECV", 8, 5000);
            } else { 
				flow_step = STEP_FAIL; 
			}
            break;

        case STEP_SUB_DESIRED_GET:
            if (result) {
                /* 解析服务器返回的期望参数 */
                char *json = extract_content((char*)From4G);
                if (json != NULL) {
                    unsigned char uchFlashCnt = 0;
                    if (get_json_value(json)  == 0) /* HeartFreq */ 
					{
						if(stu_flash.updateIntervalmins != stu_flashjson.updateIntervalmins)
						{
							stu_flash.updateIntervalmins = stu_flashjson.updateIntervalmins;
							uchFlashCnt ++;
						}
					}
                    if (get_json_value1(json) == 0) /* Latitude  */
					{  
						if (stu_flash.unLat != stu_flashjson.unLat)
						{
							stu_flash.unLat = stu_flashjson.unLat;
							stu_flash.dbLat = stu_flashjson.dbLat;
							uchFlashCnt ++;
						}
					}
                    if (get_json_value2(json) == 0) /* Longitude */ 
					{ 
						if (stu_flash.unLon != stu_flashjson.unLon)
						{
							stu_flash.unLon = stu_flashjson.unLon;
							stu_flash.dbLon = stu_flashjson.dbLon;
							uchFlashCnt ++;
						}
					}
                    if (uchFlashCnt != 0) 
					{
						flashWriteToEEPROM();
						HAL_Delay(1000);
					}
                    free(json);
                }
                stu_4G.get_frist_Mq_flg = 2;
                stu_4G.opentop = 1;
                flow_step = STEP_DONE;
            } else {
                /* 没收到回复也算订阅完成 */
                stu_4G.get_frist_Mq_flg = 2;
                stu_4G.opentop = 1;
                flow_step = STEP_DONE;
            }
            break;
		case STEP_DONE:
            if ((stu_4G.flow_pub_pending == 1) && (at_cmd.busy == 0))
            {
                stu_4G.flow_pub_pending = 2;
                BuildPubPayload(g_pub_payload, sizeof(g_pub_payload));

                snprintf(topic, sizeof(topic),
                         "$sys/%s/%s/thing/property/post",
                         stu_flash.productKey, stu_flash.deviceName);
                snprintf(buf, sizeof(buf),
                         "AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n",
                         topic, (int)strlen(g_pub_payload));

                if (AT_Start(buf, "OK", "+QMTRECV", 8, 5000)) 
				{
                    flow_step = STEP_PUBLISH;
                }
            }
            break;
		case STEP_PUBLISH:
            if (AT_IsDone(&result)) 
			{
                flow_step = STEP_DONE;   /* 回到待机态 */
				stu_4G.flow_pub_pending = 3;
            }
            break;
        default:
            flow_step = STEP_FAIL;
            break;
    }
}

/* ============================================================
 *                      查询接口
 * ============================================================ */

FlowStep_t Flow_GetStep(void)
{
    return flow_step;
}

uint8_t Flow_IsDone(void)
{
    if (flow_step == STEP_DONE) 
	{
		return 1;
	}
    if (flow_step == STEP_FAIL) 
	{
		return 2;
	}
    return 0;
}

/* ============================================================
 *                      JSON 解析
 * ============================================================ */

/**
 * @brief  从 From4G 里截取第一个 { 到最后一个 } 之间的 JSON 字符串
 * @retval 成功返回 malloc 的字符串（调用者负责 free），失败返回 NULL
 */
char* extract_content(const char* str)
{
    char *start = strchr(str, '{');
    if (start == NULL) 
	{
		return NULL;
	}

    char *end = strrchr(str, '}');
    if (end == NULL) 
	{
		return NULL;
	}

    int length = end - start + 1;
    char *middle = (char*)malloc((length + 1) * sizeof(char));
    if (middle == NULL) 
	{
		return NULL;
	}

    strncpy(middle, start, length);
    middle[length] = '\0';
    return middle;
}

/**
 * @brief  解析 HeartFreq
 * @note   服务器下发格式：
 *         {"data":{"HeartFreq":{"value":12,...}}}
 */
int get_json_value(const char* json_str)
{
	unsigned char uchCount = 0;

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) 
	{
		return 1;
	}

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *heartFreq = cJSON_GetObjectItem(data, "HeartFreq");
    if (heartFreq == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *fre = cJSON_GetObjectItem(heartFreq, "value");
    if (fre == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    /* 写入 stu_flashjson */
    uchCount = fre->valueint;
    if (uchCount > 0) 
	{
        stu_flashjson.updateIntervalmins = 24 * 60 / uchCount;
    }

    cJSON_Delete(root);
    return 0;
}

/**
 * @brief  解析 Latitude
 * @note   服务器下发格式：
 *         {"data":{"Latitude":{"value":31.1234,...}}}
 */
int get_json_value1(const char* json_str)
{
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) 
	{
		return 1;
	}

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *Latitude = cJSON_GetObjectItem(data, "Latitude");
    if (Latitude == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *lat = cJSON_GetObjectItem(Latitude, "value");
    if (lat == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    /* 写入 stu_flashjson */
    stu_flashjson.dbLat = lat->valuedouble;
    stu_flashjson.unLat = (unsigned int)(stu_flashjson.dbLat * 10000);

    cJSON_Delete(root);
    return 0;
}

/**
 * @brief  解析 Longitude
 * @note   服务器下发格式：
 *         {"data":{"Longitude":{"value":118.5678,...}}}
 */
int get_json_value2(const char* json_str)
{
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) 
	{
		return 1;
	}

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *Longitude = cJSON_GetObjectItem(data, "Longitude");
    if (Longitude == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    cJSON *lon = cJSON_GetObjectItem(Longitude, "value");
    if (lon == NULL) 
	{ 
		cJSON_Delete(root); 
		return 1; 
	}

    /* 写入 stu_flashjson */
    stu_flashjson.dbLon = lon->valuedouble;
    stu_flashjson.unLon = (unsigned int)(stu_flashjson.dbLon * 10000);

    cJSON_Delete(root);
    return 0;
}

/* ============================================================
 *                      构建上报 JSON
 * ============================================================ */

static void BuildPubPayload(char *payload, uint16_t size)
{
    cJSON *root   = cJSON_CreateObject();
    cJSON *params = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "id", "12");
    cJSON_AddStringToObject(root, "version", "1.0");

	if (stu_flag.bAccFlag)
	{
		cJSON *CoverStatus_obj = cJSON_CreateObject();
		cJSON_AddBoolToObject(CoverStatus_obj, "value", stu_flag.bAccFlag);
		cJSON_AddItemToObject(params, "CoverStatus", CoverStatus_obj);
	}

    /* CH4 */
    if (stu_sensor.usCH4 > 100) 
	{
		stu_sensor.usCH4 = 100;
	}
    cJSON *CH4_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(CH4_obj, "value", stu_sensor.usCH4);
    cJSON_AddItemToObject(params, "CH4", CH4_obj);

    /* H2S */
    stu_sensor.usH2S = getusData(stu_sensor.sH2S);
    if (stu_sensor.usH2S > 100) 
	{
		stu_sensor.usH2S = 100;
	}
    cJSON *H2S_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(H2S_obj, "value", stu_sensor.usH2S);
    cJSON_AddItemToObject(params, "H2S", H2S_obj);

    /* 雷达 */
    stu_sensor.usLD = getusData(stu_sensor.sLD);
    cJSON *Radar_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Radar_obj, "value", stu_sensor.usLD);
    cJSON_AddItemToObject(params, "Radar", Radar_obj);

    /* 电池 */
    if (stu_sensor.batter > 100) 
	{
		stu_sensor.batter = 100;
	}
    cJSON *Battery_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Battery_obj, "value", stu_sensor.batter);
    cJSON_AddItemToObject(params, "Battery", Battery_obj);

    /* 经度 */
    if (stu_flash.dbLon < 0)   
	{
		stu_flash.dbLon = 0;
	}
    if (stu_flash.dbLon > 180) 
	{
		stu_flash.dbLon = 180;
	}
    cJSON *Longitude_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Longitude_obj, "value", stu_flash.dbLon);
    cJSON_AddItemToObject(params, "Longitude", Longitude_obj);

    /* 纬度 */
    if (stu_flash.dbLat < 0)  
	{
		stu_flash.dbLat = 0;
	}
    if (stu_flash.dbLat > 90) 
	{
		stu_flash.dbLat = 90;
	}
    cJSON *Latitude_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Latitude_obj, "value", stu_flash.dbLat);
    cJSON_AddItemToObject(params, "Latitude", Latitude_obj);

    cJSON_AddItemToObject(root, "params", params);

    /* 序列化 */
    char *json_str = cJSON_PrintUnformatted(root);
    memset(payload, 0, size);
    strncpy(payload, json_str, size - 1);
    free(json_str);
    cJSON_Delete(root);
}

/* ============================================================
 *                      按需上报
 * ============================================================ */

/**
 * @brief  请求上报一次
 * @note   只置标志位，真正的上报在 Flow_Task 的 STEP_DONE 里执行
 *         流程还没走完时调用也没关系，标志位会一直挂着，
 *         等流程完成进入 STEP_DONE 后自动执行
 */
void Mqtt_RequestPublish(void)
{
    if (stu_4G.InitFlag != 1) return;   /* MQTT 未连接 */
    if (!stu_4G.opentop) return;        /* 未订阅完成 */

    stu_4G.flow_pub_pending = 1;
}



