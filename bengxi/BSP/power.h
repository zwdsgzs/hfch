#ifndef _POWER_H_
#define _POWER_H_
#include "config.h"


void Interrupt_Off(void);

void Interrupt_ON(void);
void NbIot_PowerOn(void);
void NbIot_PowerOff(void);
void Sensor_PowerAllOn(void);	
void Sensor_PowerAllOff(void);
void Sensor_PowerOn(void);
void Sensor_PowerOff(void);
void sleep_pin_Power_cfg(void);
void Start_power_MOTOR(void);
void Start_get_SN(void);


void Sleep(void);
void Wake(void);
#endif

