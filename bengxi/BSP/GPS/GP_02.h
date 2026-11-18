#ifndef __GP_02_H
#define __GP_02_H

#include "config.h"

extern bool gps_ok;

void GP_02_Rcv_One_Char(unsigned char u16Data);

int parseAndValidateGNGLL( char *nmeaSentence, double *latitude, double *longitude);

unsigned char containsGNGLL(const char *str);

double Get_latitude(void);
double Get_longitude(void);
unsigned char  Get_GPSOK_Flag(void);

double dmm_to_dd(double dmm);

#endif 

