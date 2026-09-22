#ifndef __RADAR_H
#define __RADAR_H

#include "main.h"

#define radarLength	7

void Radarrcv(void);
void praseRadarData(unsigned char uchData[]);
void LD8001_Read(void);

#endif 
