#include "EC800M.h"

stu4GDef stu_4G = {0};

extern UART_HandleTypeDef huart1;       /* 4G 模块串口*/
static unsigned char EC800M_UART_Send(const uint8_t *data, uint16_t len);
static unsigned char EC800M_Publish  (const char *topic, const char *payload);

/* ============================================================================
 *                              接收解析
 * ========================================================================== */

/**
 * @brief  从 rb_usart1 搬数据到 stu_4G.From4G，检测 \r\n
 * @note   你的 HAL_UARTEx_RxEventCallback 已经负责把数据写入 rb_usart1，
 *         这里只负责"消费"它。
 */
void EC800M_Poll_Rx(void)
{
    uint8_t  tmp[RING_BUF_SIZE];
    uint16_t n = ring_buf_read(&rb_usart1, tmp, sizeof(tmp));
    static unsigned char Data[2] = {0};
#if (DEBUG_UART5 == 1)
    static bool rcv_prefix_printed = false;   /* 本帧是否已打过 "rcv: " */
#endif

    for (uint16_t i = 0; i < n; i++)
    {
        unsigned char ch = tmp[i];

        Data[0] = Data[1];
        Data[1] = ch;

        if (stu_4G.From4G_N < FROM4G_BUF_SIZE) {
            stu_4G.From4G[stu_4G.From4G_N++] = (char)ch;
        }

#if (DEBUG_UART5 == 1)
        /* ---------- 调试打印 rcv ---------- */
        if (!rcv_prefix_printed) {
            printf("rcv: ");
            rcv_prefix_printed = true;
        }
        printf("%c", ch);

        /* 遇到 \n 认为一帧结束，下帧重新打 "rcv: " */
        if (ch == '\n') {
            rcv_prefix_printed = false;
        }
#endif

        if (Data[0] == 0x0D && Data[1] == 0x0A) {
            stu_4G.isFrom4G_OK++;
        }
    }
}

/**
 * @brief  4G 串口发送（阻塞等待上一次发送完成，然后走 uartSendIT）
 * @retval 1=成功，0=失败
 */
static unsigned char EC800M_UART_Send(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) return 0;

    /* 等上一次中断发送完成（最长 1s） */
	for (int i = 0; i < 200; i++) 
	{     // 200 * 5ms = 1000ms
		if (huart1.gState == HAL_UART_STATE_READY) break;
		osDelay(5);
	}

    return (uartSendIT(1, data, len) == HAL_OK) ? 1 : 0;   /* 用串口1发送 */
}

/* ============================================================================
 *                              AT 命令状态机
 * ========================================================================== */

/**
 * @brief  发送 AT 命令并等待一个关键字
 */
unsigned char AT_Send(char *str, char *Find, unsigned char errN)
{
    AT_State state = STATE_SEND_CMD;
    int      retry = 0;

    while (retry < errN)
    {
        switch (state)
        {
            case STATE_SEND_CMD:
#if (DEBUG_UART5 == 1)
                printf("send: %s", str);
#endif
                if (EC800M_UART_Send((uint8_t *)str, strlen(str)) == 0) {
                    retry++;
                    break;
                }

                /* 发送成功后再清接收侧（避免误删模块刚回的响应） */
                ring_buf_clear(&rb_usart1);
                memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
                stu_4G.From4G_N   = 0;
                stu_4G.isFrom4G_OK = 0;

                state = STATE_WAIT_RESPONSE;
                break;

			case STATE_WAIT_RESPONSE:
				for (int i = 0; i < 36; i++) {     // 36 * 50ms = 1800ms
					EC800M_Poll_Rx();
					if (stu_4G.isFrom4G_OK) break;   // 收到完整帧立刻退出
					osDelay(50);                   // ★ 必须让出 CPU
				}
                state = STATE_CHECK_RESPONSE;
                break;

            case STATE_CHECK_RESPONSE:
                EC800M_Poll_Rx();
                if (strstr((const char *)stu_4G.From4G, (const char *)Find)) {
                    return 1;
                } else {
                    retry++;
                    state = STATE_SEND_CMD;
                }
                break;

            default: break;
        }
    }
    return 0;
}

/**
 * @brief  发送 AT 命令并等待两个关键字同时出现
 * @param  str    要发送的 AT 命令
 * @param  Find1  关键字 1
 * @param  Find2  关键字 2
 * @param  errN   最大重试次数
 * @retval 1=匹配成功，0=失败
 */
unsigned char AT_Send_2(char *str, char *Find1, char *Find2, unsigned char errN)
{
    AT_State state = STATE_SEND_CMD;
    int      retry = 0;

    while (retry < errN)
    {
        switch (state)
        {
            case STATE_SEND_CMD:
                printf("%s", str);

                if (EC800M_UART_Send((uint8_t *)str, strlen(str)) == 0) {
                    retry++;
                    break;
                }

                ring_buf_clear(&rb_usart1);
                memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
                stu_4G.From4G_N   = 0;
                stu_4G.isFrom4G_OK = 0;

                state = STATE_WAIT_RESPONSE;
                break;

            case STATE_WAIT_RESPONSE:
                for (int i = 0; i < 36; i++) {     // 36 * 50ms = 1800ms
					EC800M_Poll_Rx();
					if (stu_4G.isFrom4G_OK) break;   // 收到完整帧立刻退出
					osDelay(50);                   // ★ 必须让出 CPU
				}
                state = STATE_CHECK_RESPONSE;
                break;

            case STATE_CHECK_RESPONSE:
                EC800M_Poll_Rx();
                if (strstr((const char *)stu_4G.From4G, (const char *)Find1) &&
                    strstr((const char *)stu_4G.From4G, (const char *)Find2)) {
                    return 1;
                } else {
                    retry++;
                    state = STATE_SEND_CMD;
                }
                break;

            default: break;
        }
    }
    return 0;
}

/* ============================================================================
 *                          EC800M 两步式发布消息
 * ========================================================================== */

/**
 * @brief  EC800M 发布 MQTT 消息（AT+QMTPUBEX 两步式）
 * @retval 1=成功，0=失败
 */
static unsigned char EC800M_Publish(const char *topic, const char *payload)
{
    char cmd[AT_BUF_SIZE];
    int  len = (int)strlen(payload);

    /* 步骤 1：发命令 + 长度 */
    snprintf(cmd, sizeof(cmd),
             "AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n", topic, len);

    ring_buf_clear(&rb_usart1);
    memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
    stu_4G.From4G_N   = 0;
    stu_4G.isFrom4G_OK = 0;

    if (EC800M_UART_Send((uint8_t *)cmd, strlen(cmd)) == 0) return 0;

    /* 等待 '>' 提示符 */
	for (int i = 0; i < 60; i++) 
	{     // 60 * 50ms = 3000ms
		EC800M_Poll_Rx();
		if (strchr(stu_4G.From4G, '>')) break;
		osDelay(50);
	}

    if (!strchr(stu_4G.From4G, '>')) return 0;

    /* 步骤 2：发 payload */
    if (EC800M_UART_Send((uint8_t *)payload, len) == 0) return 0;

    /* 等待发布结果 */
	for (int i = 0; i < 160; i++) 
	{     // 160 * 50ms = 8000ms
		EC800M_Poll_Rx();
		if (strstr(stu_4G.From4G, "+QMTPUBEX") ||
            strstr(stu_4G.From4G, "+QMTRECV")) {
            return 1;
        }
		osDelay(50);
	}

    return 0;
}

/* ============================================================================
 *                          获取模块 ICCID / SN
 * ========================================================================== */

uint8_t GET_4G_ID_SN(void)
{
    unsigned char Result = 0;
    unsigned char i, j;

    /* ---------- 获取 ICCID（EC800M 用 AT+QCCID） ---------- */
    Result = AT_Send("AT+QCCID\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    if (Result == 1)
    {
        j = 0;
        memset(stu_4G.mqttClientID, 0, sizeof(stu_4G.mqttClientID));
        for (i = 10; i < 32; i++)
        {
            if ((stu_4G.From4G[i] >= '0' && stu_4G.From4G[i] <= '9') ||
                (stu_4G.From4G[i] >= 'A' && stu_4G.From4G[i] <= 'Z'))
            {
                stu_4G.mqttClientID[j++] = stu_4G.From4G[i];
            }
        }
    }

    /* ---------- 获取 IMEI/SN ---------- */
    Result = AT_Send("AT+CGSN=1\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    if (Result == 1)
    {
        j = 0;
        memset(stu_4G.mqttClientSN, 0, sizeof(stu_4G.mqttClientSN));
        for (i = 0; i < stu_4G.From4G_N; i++)
        {
            if (stu_4G.From4G[i] >= '0' && stu_4G.From4G[i] <= '9')
            {
                stu_4G.mqttClientSN[j++] = stu_4G.From4G[i];
            }
        }
    }

    return 1;

Mqtt_Fail:
    AT_Send("AT+CFUN=1,1\r\n", "OK", 2);
    return 0;
}

/* ============================================================================
 *                              4G 初始化
 * ========================================================================== */

unsigned char Init_4G(void)
{
    unsigned char Result = 0;
    unsigned char buff[20];
    uint8_t       count  = 20;
    int  ret = 0;

    /* ---------- 1. AT 测试 ---------- */
    Result = AT_Send("AT\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 2. 获取 ICCID / SN ---------- */
    GET_4G_ID_SN();

    /* ---------- 3. 查询信号质量 ---------- */
    stu_4G.notsigal = true;
    do {
        if (count > 20) break;

        Result = AT_Send("AT+CSQ\r\n", "CSQ", 10);
        if (Result == 0) goto Mqtt_Fail;

        if (Result == 1)
        {
            memset(buff, 0, sizeof(buff));
            if (strstr((const char *)stu_4G.From4G, "+CSQ: "))
            {
                char *index = strstr((const char *)stu_4G.From4G, "+CSQ: ");
                buff[0] = *(index + 6);
                buff[1] = *(index + 7);
                stu_4G.CSQ = atoi((const char *)buff);
            }
        }
        if ((stu_4G.CSQ > 1) && (stu_4G.CSQ < 99)) {
            stu_4G.notsigal = false;
            break;
        }
    } while (count--);

    if (stu_4G.notsigal) return 0;

    if (stu_4G.CSQ == 99 || stu_4G.CSQ < 12) {
        stu_4G.CSQ = 15;
    }

    /* ---------- 4. 断开旧的 MQTT 连接 ---------- */
    AT_Send("AT+QMTDISC=0\r\n",  "OK", 1);
    osDelay(1000);
    AT_Send("AT+QMTCLOSE=0\r\n", "OK", 1);
    osDelay(1000);

    /* ---------- 5. 射频 / 网络注册 ---------- */
    Result = AT_Send("AT+CFUN=1\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;
    Result = AT_Send("AT+CEREG?\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;
    Result = AT_Send("AT+CREG=1\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;

    /* ---------- 6. 配置 MQTT 会话 ---------- */
	sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"version\",0,4\r\n");
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;
    sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"recv/mode\",0,0,1\r\n");
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;
    sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"onenet\",0,\"%s\",\"%s\"\r\n",
                    stu_flash.productKey,
                    stu_flash.deviceSecret);
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 7. 打开 MQTT 网络 ---------- */
    memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
    sprintf(stu_4G.AT_Buff, "AT+QMTOPEN=0,\"%s\",1883\r\n", stu_flash.mqttBroker);
    Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTOPEN", 8);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 8. 生成 OneNET Token 并连接 ---------- */
    {
        if (ret > 0)
        {
            memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
            sprintf(stu_4G.AT_Buff, "AT+QMTCONN=0,\"%s\"\r\n",
                    stu_flash.deviceName);
            Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTCONN", 8);
            if (Result == 0) goto Mqtt_Fail;
        }
    }

    if (Result == 1)
    {
        stu_4G.InitFlag = 1;
        return 1;
    }

Mqtt_Fail:
    stu_4G.InitFlag = 0;
    AT_Send("AT+CFUN=1,1\r\n", "OK", 2);
    return 0;
}

/* ============================================================================
 *                              4G 重连
 * ========================================================================== */

unsigned char reConnect_4G(void)
{
    unsigned char Result = 0;
    unsigned char buff[20];

    int  ret = 0;

    stu_4G.notsigal = true;
    stu_4G.CSQ      = 0;

    for (int i = 0; i < 10; ++i)
    {
        Result = AT_Send("AT+CSQ\r\n", "CSQ", 10);
        if (Result == 0) goto Mqtt_Fail;

        if (Result == 1)
        {
            memset(buff, 0, sizeof(buff));
            if (strstr((const char *)stu_4G.From4G, "+CSQ: "))
            {
                char *index = strstr((const char *)stu_4G.From4G, "+CSQ: ");
                buff[0] = *(index + 6);
                buff[1] = *(index + 7);
                stu_4G.CSQ = atoi((const char *)buff);
            }
        }
        if ((stu_4G.CSQ > 1) && (stu_4G.CSQ < 99)) {
            stu_4G.notsigal = false;
            break;
        }
    }
    if (stu_4G.notsigal) return 0;

    memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
    sprintf(stu_4G.AT_Buff, "AT+QMTOPEN=0,\"%s\",1883\r\n", stu_flash.mqttBroker);
    Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTOPEN", 3);
    if (Result == 0) goto Mqtt_Fail;
	
    {
        if (ret > 0)
        {
            memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
            sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"onenet\",0,\"%s\",\"%s\"\r\n",
                    stu_flash.productKey,
                    stu_flash.deviceSecret);
            Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTCONN", 3);
            if (Result == 0) goto Mqtt_Fail;
        }
    }

    if (Result == 1) {
        stu_4G.InitFlag = 1;
        return 1;
    }

Mqtt_Fail:
    stu_4G.InitFlag = 0;
    AT_Send("AT+CFUN=1,1\r\n", "OK", 2);
    return 0;
}

/* ============================================================================
 *                          MQTT 业务：订阅影子
 * ========================================================================== */

void Mq_Sub_shadow(void)
{
    char Topic[200], Topic1[200];
    unsigned char payload[80] =
        "{\"id\":\"12\",\"version\":\"1.0\",\"params\":[\"HeartFreq\",\"Latitude\",\"Longitude\"]}";

    unsigned char uchFlashCnt = 0;

    if (stu_4G.get_frist_Mq_flg == 2)
    {
        memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
        memset(Topic1, 0, sizeof(Topic1));
        sprintf(Topic1, "$sys/%s/%s/thing/property/desired/get/reply",
                stu_flash.productKey, stu_flash.deviceName);
        sprintf(stu_4G.AT_Buff, "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic1);
        AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8);
        stu_4G.get_frist_Mq_flg++;
    }

    memset(Topic, 0, sizeof(Topic));
    sprintf(Topic, "$sys/%s/%s/thing/property/desired/get",
            stu_flash.productKey, stu_flash.deviceName);

    if (EC800M_Publish(Topic, (const char *)payload) == 0) {
        return;
    }

    char *result = extract_content(stu_4G.From4G);
    if (result != NULL)
    {
        if (get_json_value(result) == 0) {
            if (stu_flash.updateIntervalmins != stu_flashjson.updateIntervalmins) {
                stu_flash.updateIntervalmins = stu_flashjson.updateIntervalmins;
                uchFlashCnt++;
            }
        }
        if (get_json_value1(result) == 0) {
            if (stu_flash.unLat != stu_flashjson.unLat) {
                stu_flash.unLat = stu_flashjson.unLat;
                stu_flash.dbLat = stu_flashjson.dbLat;
                uchFlashCnt++;
            }
        }
        if (get_json_value2(result) == 0) {
            if (stu_flash.unLon != stu_flashjson.unLon) {
                stu_flash.unLon = stu_flashjson.unLon;
                stu_flash.dbLon = stu_flashjson.dbLon;
                uchFlashCnt++;
            }
        }
        if (uchFlashCnt != 0) {
            flashWriteToEEPROM();
            osDelay(4000);
        }
        memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
        free(result);
    }
}

/* ============================================================================
 *                          MQTT 业务：上报属性
 * ========================================================================== */

void Mq_Pub_P(void)
{
    char Topic[200];
    char payload[400];
    osDelay(1000);

    if (stu_4G.get_frist_Mq_flg == 1)
    {
        if (!stu_4G.opentop)
        {
            memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
            memset(Topic, 0, sizeof(Topic));
            sprintf(Topic, "$sys/%s/%s/thing/property/post/reply",
                    stu_flash.productKey, stu_flash.deviceName);
            sprintf(stu_4G.AT_Buff, "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic);
            if (AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8) == 0) {
                return;
            }
        }
        stu_4G.get_frist_Mq_flg++;
    }

    cJSON *root   = cJSON_CreateObject();
    cJSON *params = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "id", "12");
    cJSON_AddStringToObject(root, "version", "1.0");

    if (stu_sensor.usCH4 > 100) stu_sensor.usCH4 = 100;
    cJSON *CH4_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(CH4_obj, "value", stu_sensor.usCH4);
    cJSON_AddItemToObject(params, "CH4", CH4_obj);

    stu_sensor.usH2S = getusData(stu_sensor.sH2S);
    if (stu_sensor.usH2S > 100) stu_sensor.usH2S = 100;
    cJSON *H2S_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(H2S_obj, "value", stu_sensor.usH2S);
    cJSON_AddItemToObject(params, "H2S", H2S_obj);

    stu_sensor.usLD = getusData(stu_sensor.sLD);
    if (stu_sensor.usLD == 0) stu_sensor.uchLDCnt++;
    else                      stu_sensor.uchLDCnt = 0;
    cJSON *Radar_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Radar_obj, "value", stu_sensor.usLD);
    cJSON_AddItemToObject(params, "Radar", Radar_obj);

    if (stu_sensor.batter > 100) stu_sensor.batter = 100;
    cJSON *Battery_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Battery_obj, "value", stu_sensor.batter);
    cJSON_AddItemToObject(params, "Battery", Battery_obj);

    if (stu_flash.dbLon < 0)   stu_flash.dbLon = 0;
    if (stu_flash.dbLon > 180) stu_flash.dbLon = 180;
    cJSON *Longitude_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Longitude_obj, "value", stu_flash.dbLon);
    cJSON_AddItemToObject(params, "Longitude", Longitude_obj);

    if (stu_flash.dbLat < 0)  stu_flash.dbLat = 0;
    if (stu_flash.dbLat > 90) stu_flash.dbLat = 90;
    cJSON *Latitude_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(Latitude_obj, "value", stu_flash.dbLat);
    cJSON_AddItemToObject(params, "Latitude", Latitude_obj);

    cJSON_AddItemToObject(root, "params", params);

    char *json_str = cJSON_PrintUnformatted(root);
    memset(payload, 0, sizeof(payload));
    strcat(payload, json_str);
    free(json_str);
    cJSON_Delete(root);

    memset(Topic, 0, sizeof(Topic));
    sprintf(Topic, "$sys/%s/%s/thing/property/post",
            stu_flash.productKey, stu_flash.deviceName);

    if ((stu_sensor.uchLDCnt == 0) || (stu_sensor.uchLDCnt == 3))
    {
        EC800M_Publish(Topic, payload);
        if (stu_sensor.uchLDCnt == 3) {
            stu_sensor.uchLDCnt = 0;
        }
    }
}

/* ============================================================================
 *                          MQTT 业务：上报开盖状态
 * ========================================================================== */

void Mq_Pub_open(void)
{
    char Topic[200];
    char payload[400];
    osDelay(1000);

    if (!stu_4G.opentop)
    {
        memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
        memset(Topic, 0, sizeof(Topic));
        sprintf(Topic, "$sys/%s/%s/thing/property/post/reply",
                stu_flash.productKey, stu_flash.deviceName);
        sprintf(stu_4G.AT_Buff, "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic);
        if (AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8) == 0) {
            return;
        }
        stu_4G.opentop = true;
    }

    cJSON *root   = cJSON_CreateObject();
    cJSON *params = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "id", "12");
    cJSON_AddStringToObject(root, "version", "1.0");

    cJSON *CoverStatus_obj = cJSON_CreateObject();
    cJSON_AddBoolToObject(CoverStatus_obj, "value", stu_flag.bUpport);
    cJSON_AddItemToObject(params, "CoverStatus", CoverStatus_obj);

    cJSON_AddItemToObject(root, "params", params);

    char *json_str = cJSON_PrintUnformatted(root);
    memset(payload, 0, sizeof(payload));
    strcat(payload, json_str);
    free(json_str);
    cJSON_Delete(root);

    memset(Topic, 0, sizeof(Topic));
    sprintf(Topic, "$sys/%s/%s/thing/property/post",
            stu_flash.productKey, stu_flash.deviceName);

    EC800M_Publish(Topic, payload);
}

/* ============================================================================
 *                              JSON 解析
 * ========================================================================== */

char* extract_content(const char* str)
{
    char *start = strchr(str, '{');
    if (start == NULL) return NULL;

    char *end = strrchr(str, '}');
    if (end == NULL) return NULL;

    int length = end - start + 1;

    char* middle = (char*)malloc((length + 1) * sizeof(char));
    if (middle == NULL) return NULL;

    strncpy(middle, start, length);
    middle[length] = '\0';
    return middle;
}

int get_json_value(const char* json_str)
{
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) return 1;

    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) { cJSON_Delete(root); return 1; }

    cJSON* heartFreq = cJSON_GetObjectItem(data, "HeartFreq");
    if (heartFreq == NULL) { cJSON_Delete(root); return 1; }

    cJSON* fre = cJSON_GetObjectItem(heartFreq, "value");
    if (fre == NULL) { cJSON_Delete(root); return 1; }

    cJSON_Delete(root);
    stu_sensor.uchReportCount = fre->valueint;
	if (stu_sensor.uchReportCount < 2)
	{
		stu_sensor.uchReportCount = 2;
	}
	if (stu_sensor.uchReportCount > 24)
	{
		stu_sensor.uchReportCount = 24;
	}
	stu_flashjson.updateIntervalmins = 24 * 60 / stu_sensor.uchReportCount;
    return 0;
}

/**
 * @brief  解析影子 JSON：纬度 Latitude
 * @retval 0=成功，1=失败
 */
int get_json_value1(const char* json_str)
{
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) return 1;

    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) { cJSON_Delete(root); return 1; }

    cJSON* Latitude = cJSON_GetObjectItem(data, "Latitude");
    if (Latitude == NULL) { cJSON_Delete(root); return 1; }

    cJSON* lat = cJSON_GetObjectItem(Latitude, "value");
    if (lat == NULL) { cJSON_Delete(root); return 1; }

    cJSON_Delete(root);
    stu_flashjson.dbLat = lat->valuedouble;
    stu_flashjson.unLat = (unsigned int)(stu_flashjson.dbLat * 10000);

    return 0;
}

/**
 * @brief  解析影子 JSON：经度 Longitude
 * @retval 0=成功，1=失败
 */
int get_json_value2(const char* json_str)
{
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) return 1;

    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) { cJSON_Delete(root); return 1; }

    cJSON* Longitude = cJSON_GetObjectItem(data, "Longitude");
    if (Longitude == NULL) { cJSON_Delete(root); return 1; }

    cJSON* lon = cJSON_GetObjectItem(Longitude, "value");
    if (lon == NULL) { cJSON_Delete(root); return 1; }

    cJSON_Delete(root);
    stu_flashjson.dbLon = lon->valuedouble;
    stu_flashjson.unLon = (unsigned int)(stu_flashjson.dbLon * 10000);

    return 0;
}



