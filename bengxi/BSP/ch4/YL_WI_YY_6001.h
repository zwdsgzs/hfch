#ifndef __YL_WI_YY_6001_H
#define __YL_WI_YY_6001_H

#include "config.h"
extern float CH4_percent ,CH4_Kpa ,CH4_Temp ,CH4_per_num,CH4_count;
void YL_WI_YY_6001_Rcv_One_Char(unsigned char Data);
void YL_WI_YY_6001_Handle(void);

int BCDToDecimal(int bcd);
float str_to_float(const unsigned char *str);
unsigned int hex_string_to_int(const unsigned char *hex_str);
double round_to_two_decimal_places(float number);
double Get_CH4_percent(void);
double Get_CH4_Kpa(void);
double Get_CH4_Temp(void);
unsigned int Get_CH4(void);
unsigned int Get_Temp(void);
#endif 
