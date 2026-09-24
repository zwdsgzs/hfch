#ifndef __EC800M_H
#define __EC800M_H

#include "main.h"
#include "token.h"

extern unsigned char From4G[600];

/* ============================================================
 *                      AT 状态机
 * ============================================================ */

typedef enum {
    AT_STATE_IDLE = 0,
    AT_STATE_SEND,
    AT_STATE_WAIT,
    AT_STATE_CHECK
} AT_State_t;

typedef struct {
    AT_State_t  state;          /* 当前状态 */
    char        cmd[256];       /* 待发送的命令 */
    char        find1[32];      /* 关键字1 */
    char        find2[32];      /* 关键字2（可选） */
    uint8_t     need_two;       /* 是否需要两个关键字 */
    uint8_t     retry;          /* 剩余重试次数 */
    uint8_t     result;         /* 结果：0=失败，1=成功 */
    uint8_t     busy;           /* 1=正在执行，0=空闲 */
    uint32_t    tick_start;     /* 发送时刻 */
    uint32_t    timeout_ms;     /* 超时时间 */
} AT_Cmd_t;
extern AT_Cmd_t at_cmd;

/* ============================================================
 *                      流程步骤枚举
 * ============================================================ */

typedef enum {
    STEP_IDLE = 0,          /* 空闲，还没开始 */
    STEP_AT,                /* AT 测试 */
    STEP_QCCID,             /* 获取 ICCID */
    STEP_CGSN,              /* 获取 IMEI */
    STEP_CSQ,               /* 信号质量 */
    STEP_QMTDISC,           /* 断开 MQTT */
    STEP_QMTCLOSE,          /* 关闭 MQTT 网络 */
    STEP_CFUN,              /* 全功能模式 */
    STEP_CEREG,             /* 查询 EPS 注册 */
    STEP_CREG,              /* 使能 CS 域 URC */
    STEP_QMTCFG_CLOUD,      /* OneNET 配置 */
    STEP_QMTCFG_SESSION,    /* 会话配置 */
    STEP_QMTOPEN,           /* 打开 MQTT 网络 */
    STEP_QMTCONN,           /* 连接 MQTT 服务器 */
    STEP_SUB_POST_REPLY,    /* 订阅 post/reply */
    STEP_SUB_DESIRED_REPLY, /* 订阅 desired/get/reply */
    STEP_SUB_DESIRED_GET,   /* 发布 desired/get 请求 */
    STEP_PUBLISH,           /* 上报 */
    STEP_DONE,              /* 全部完成 */
    STEP_FAIL               /* 失败 */
} FlowStep_t;

/* ============================================================
 *                      状态结构体
 * ============================================================ */
typedef struct {
	unsigned char   isFrom4G_OK;    	/* 收到 \r\n 的次数 */
    unsigned short  From4G_N;       	/* From4G 中有效数据长度 */
    unsigned char   CSQ;            	/* 信号质量 */
    unsigned char   InitFlag;       	/* 初始化完成标志 1=已完整初始化过*/
    unsigned char   mqttClientSN[30];   /* IMEI */
    unsigned char   mqttClientID[50];   /* ICCID */
    unsigned int    get_frist_Mq_flg;   /* MQTT 首次订阅标志 */
    unsigned char   opentop;            /* 是否已订阅 post/reply */
    unsigned char   notsigal;           /* 无信号标志 */

	uint8_t    flow_csq_retry;			//信号质量重试计数
	uint8_t    flow_pub_pending;		//上报请求标志
} stu4GDef;
extern stu4GDef stu_4G;

/* ============================================================
 *                      API
 * ============================================================ */

// 从环形缓冲区搬数据到 From4G 
void EC800M_ProcessRxData(void);

// 启动一个 AT 命令（非阻塞） 
uint8_t AT_Start(char *str, char *find1, char *find2,
                 uint8_t errN, uint32_t timeout_ms);

// AT 状态机，主循环里每轮调用 
void AT_Task(void);

// 检查 AT 命令是否完成 
uint8_t AT_IsDone(uint8_t *pResult);

/* 流程状态机 */
void Flow_Start(void);              /* 启动流程 */
void Flow_Task(void);               /* 主循环里调 */
FlowStep_t Flow_GetStep(void);      /* 查询当前步骤（调试用） */
uint8_t Flow_IsDone(void);          /* 0=进行中，1=成功，2=失败 */

char* extract_content(const char* str);
int get_json_value(const char* json_str);
int get_json_value1(const char* json_str);
int get_json_value2(const char* json_str);

/* 上报请求 */
static void BuildPubPayload(char *payload, uint16_t size);
void Mqtt_RequestPublish(void);

#endif
