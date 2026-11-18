#ifndef __CONFIG_H
#define __CONFIG_H


#define LOG_ON  1

#define BIT(x)	           (1 << (x))
#define SETBIT(x, y)   	   (x |= y)
#define CLEARBIT(x, y) 	   (x &= ~y)
#define CHECKBIT(x, y) 	   (x & y)

#define BCD2BIN(val)  (((val) & 0x0f) + ((val)>>4)*10) 
#define BIN2BCD(val)  ((((val)/10)<<4) + (val)%10)


#define software_version 26062402
#define hardware_version  "V1.0"



// ***** Define I/O pins *****
#define BIT7 0x80
#define BIT6 0x40
#define BIT5 0x20
#define BIT4 0x10
#define BIT3 0x08
#define BIT2 0x04
#define BIT1 0x02
#define BIT0 0x01



#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include <math.h>
#include <stdlib.h>
#include <cJSON.h>

#include "main.h"

#include "GP_02.h"
#include "EC_01.h"
#include "PS_H2S_100.h"
#include "YL_WI_YY_6001.h"
#include "flash.h"
#include "power.h"
#include "HLK-LD2413.h"
#include "CRC_SUM.h"
#include "hardware.h"
#include "dataproce.h"
#include "ADC.h"
#include "TTL.h"

#endif 

