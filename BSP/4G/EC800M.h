#ifndef __EC800M_H
#define __EC800M_H

#include "main.h"
#include <stdbool.h>

/* ===================== 缓冲区尺寸 ===================== */
#define FROM4G_BUF_SIZE     600     /* 4G 接收拼接缓冲大小 */
#define AT_BUF_SIZE         512     /* AT 命令拼接缓冲大小 */

/* ===================== AT 状态机 ===================== */
typedef enum {
    STATE_SEND_CMD,         /* 发送 AT 命令 */
    STATE_WAIT_RESPONSE,    /* 等待模块响应 */
    STATE_CHECK_RESPONSE    /* 校验响应内容 */
} AT_State;

/* ===================== 4G 模块全局结构体 ===================== */
typedef struct
{
    /* ---------- 接收相关 ---------- */
    char          From4G[FROM4G_BUF_SIZE];  /* 4G 模块接收拼接缓冲（从 ring buffer 搬过来） */
    unsigned int  From4G_N;                 /* From4G 当前写入位置 */
    unsigned char isFrom4G_OK;              /* 收到 \r\n 的次数（用于判断一帧结束） */

    /* ---------- AT 命令拼接缓冲 ---------- */
    char          AT_Buff[AT_BUF_SIZE];     /* 通用 AT 命令拼接缓冲 */

    /* ---------- 网络 / 状态 ---------- */
    unsigned char CSQ;                      /* 信号强度（0~31，99 表示未知） */
    unsigned char InitFlag;                 /* 4G 初始化成功标志：1=成功，0=失败 */
    bool          notsigal;                 /* 无信号标志：true=无信号，false=有信号 */
    bool          opentop;                  /* 是否已订阅 post/reply 主题 */

    /* ---------- MQTT 客户端信息 ---------- */
    unsigned char mqttClientSN[30];         /* 模块 SN（CGSN 获取） */
    unsigned char mqttClientID[50];         /* 模块 ICCID（QCCID 获取） */

    /* ---------- MQTT 流程控制 ---------- */
    unsigned int  get_frist_Mq_flg;         /* 首次 MQTT 订阅标志：0=未订阅，1=首次，2=影子，3+=正常 */

	unsigned char reported;
} stu4GDef;

extern stu4GDef stu_4G;

/* ===================== 对外接口 ===================== */
/* 主循环 / AT 等待循环中调用：把 ring buffer 数据搬到 From4G 并检测 \r\n */
void          EC800M_Poll_Rx(void);

/* AT 命令发送（状态机，阻塞式等待响应） */
unsigned char AT_Send  (char *str, char *Find,                         unsigned char errN);
unsigned char AT_Send_2(char *str, char *Find1, char *Find2,           unsigned char errN);

/* 4G 初始化 / 重连 */
unsigned char Init_4G(void);
unsigned char reConnect_4G(void);

/* 获取模块 ID / SN */
uint8_t       GET_4G_ID_SN(void);

/* MQTT 业务 */
void          Mq_Sub_shadow(void);   /* 订阅影子属性并拉取期望值 */
void          Mq_Pub_P(void);        /* 上报属性数据 */
void          Mq_Pub_open(void);     /* 上报开盖状态 */

/* JSON 解析 */
char*         extract_content(const char* str);
int           get_json_value (const char* json_str);  /* 上报频率 */
int           get_json_value1(const char* json_str);  /* 纬度 */
int           get_json_value2(const char* json_str);  /* 经度 */


#endif
