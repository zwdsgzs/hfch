#ifndef __EC_01_H
#define __EC_01_H

#include "config.h"
#include "token.h"

extern unsigned int get_frist_Mq_flg;
extern  bool opentop;

// 使用状态机模式
typedef enum {
    STATE_SEND_CMD,
    STATE_WAIT_RESPONSE,
    STATE_CHECK_RESPONSE
} AT_State;

void EC_01_Rcv_One_Char(unsigned char u16Data);
uint8_t GET_4G_ID_SN(void);
unsigned  char AT_Send(char *str,char *Find,unsigned char errN);
unsigned  char AT_Send_2(char *str,char *Find1,char *Find2,unsigned char errN);

//unsigned  char AT_Send_3(unsigned char *str,unsigned char *Find1,unsigned char *Find2,unsigned char *Find3,unsigned char errN);

unsigned char Init_4G(void);

void Mq_Sub_shadow(void);
void Mq_Pub_P(void);
void Mq_Pub_open(void);

char* extract_content(const char* str);
int get_json_value(const char* json_str);
int get_json_value1(const char* json_str);
int get_json_value2(const char* json_str);

unsigned char reConnect_4G(void);


#endif
