#ifndef __PS_H2S_100_H
#define __PS_H2S_100_H

#include "config.h"

extern unsigned int H2S ,H2S_Full ;
extern unsigned int H2S_max,H2S_count;
int Get_H2S(void);
unsigned int Get_H2S_Full(void);
void H2S_Rcv_One_Char(unsigned char Data);
void H2S_Handle(void);
//unsigned char FucCheckSum(unsigned char *i, unsigned char ln);
void H2S_SendRead(void);

#endif 
