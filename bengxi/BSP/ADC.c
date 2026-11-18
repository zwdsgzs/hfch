#include "ADC.h"

#define N 5
const unsigned char  tip_adc[]= "\n---adc fail--\n";

float Get_Adc(GET_fun_ADC_t Get_devAdc)         
{
//	 unsigned char buff[60]={0};
    uint16_t u16AdcValue=0;
	 float get_adc_distance=0;
//    int32_t iRet = LL_ERR;
//    __IO uint32_t u32TimeCount = 0UL;

    /* Can ONLY start sequence A conversion.
       Sequence B needs hardware trigger to start conversion. */
//    ADC_Start(CM_ADC1);
//	DDL_DelayMS(10);
//    do {
//        if (ADC_GetStatus(CM_ADC1, ADC_FLAG_EOCA) == SET) {
//            ADC_ClearStatus(CM_ADC1, ADC_FLAG_EOCA);
//            iRet = LL_OK;
//            break;
//        }
//    } while (u32TimeCount++ < 2000U);
//    if (iRet == LL_OK) 
	{
		if(Get_devAdc==get_battery)
		{
//			PA3_REF_RESET;
//			DDL_DelayMS(5000);
//			ADC_Start(CM_ADC1);
			 /* Get any ADC value of sequence A channel that needed. */
			u16AdcValue =filter();
			get_adc_distance = (float)u16AdcValue*3.34/4096;
			get_adc_distance = get_adc_distance*2;
			if(get_adc_distance>3.65){get_adc_distance=3.65;}
//			printf("电池--u16AdcValue=%d,Adc_distance=%.2f\r\n",u16AdcValue,get_adc_distance);
//			PA3_REF_SET;
		}
		if(Get_devAdc==WF_OUT)
		{
			 /* Get any ADC value of sequence A channel that needed. */
			u16AdcValue = ADC_GetValue(CM_ADC1, ADC_CH4);
			get_adc_distance = u16AdcValue*3.3/4096;
//			printf("N=%d\,u16AdcValue=%d,Adc_distance=%.2f\r\n",u32TimeCount,u16AdcValue,get_adc_distance);
		}
		
		if(Get_devAdc==external_5v)
		{
			 /* Get any ADC value of sequence A channel that needed. */
			u16AdcValue =ADC_GetValue(CM_ADC1, ADC_CH11);//filter();
			get_adc_distance = (float)u16AdcValue*3.34/4096;
			get_adc_distance = get_adc_distance*2;
            if(get_adc_distance>5.0){get_adc_distance=5;}
//			printf("5v--------u16AdcValue=%d,Adc_distance=%.2f\r\n",u16AdcValue,get_adc_distance);
		}
		
		ADC_Stop(CM_ADC1);
		return get_adc_distance;//round_to_two_decimal_places(get_adc_distance);
       
    } 
//	else {
//        ADC_Stop(CM_ADC1);
//		return 0;
////			  printf((const char*)tip_adc);
//    }
}


uint16_t filter(void)
{
//	 unsigned char buff[60]={0};
    uint16_t value_buf[N], temp;
    char count, i, j;
	int32_t iRet = LL_ERR;
	__IO uint32_t u32TimeCount = 0UL;
    for(count = 0; count < N; count ++) //获取采样值
    {
		ADC_Start(CM_ADC1);
		DDL_DelayMS(10);
		do {
			if (ADC_GetStatus(CM_ADC1, ADC_FLAG_EOCA) == SET) {
				ADC_ClearStatus(CM_ADC1, ADC_FLAG_EOCA);
				iRet = LL_OK;
				break;
			}
		} while (u32TimeCount++ < 2000U);
		if (iRet == LL_OK) 
		{
			value_buf[count] = ADC_GetValue(CM_ADC1, ADC_CH10);
//			printf("u16AdcValue=%d\r\n",value_buf[count]);
			DDL_DelayMS(10);
		}
		else {
			ADC_Stop(CM_ADC1);
			return 0;
		}
    }
    for(j = 0; j < (N-1); j++)
    {
        for(i = 0; i < (N-j); i++)
        {
            if(value_buf[i] > value_buf[i+1])
            {
                temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    return value_buf[(N-1)/2];
}


