#ifndef HLK_LD2413_H
#define HLK_LD2413_H
#include "config.h"

#define LADAR_HLK 0

typedef union {
    uint32_t data;
    struct {
        float Radar_Val;
    } value; 
} Radar_Data; 
 
extern uint8_t HLK_timeout; 
extern unsigned int HLK_LD2413_Distance;
void LADAR_Rcv_One_Char(unsigned char Data);
 
unsigned int Get_HLK_LD2413(void);
 
void HLK_LD2413(void);
void LD8001H(void);
void LD8001_Read(void);
 
void SetSendTime(void);
#endif
