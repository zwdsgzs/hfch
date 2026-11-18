#ifndef _PC_UART_H
#define _PC_UART_H

#include "config.h"

uint8_t PC_Handle(void);
void PC_set_mqtt(void);
void PC_set_general(void);
void PC_GET_message1(void);
void PC_GET_message2(void);
void PC_GET_P1(void);
void PC_GET_verson(void);
void TEST_MODE_GET_SERSON(void);
#endif
