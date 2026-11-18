#ifndef __ADC_H_
#define __ADC_H_

#include "config.h"

typedef enum {
	get_battery,
	WF_OUT,
	external_5v,
} GET_fun_ADC_t;

uint16_t filter(void);

float Get_Adc(GET_fun_ADC_t Get_devAdc);

#endif
