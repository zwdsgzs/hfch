#include "config.h"
#include <stdbool.h>
#include <ctype.h>
#define YL_WI_YY_6001 0


unsigned char CH4_Rcv[50],CH4_RcvN=0,CH4_RcvOK=0;
float CH4_percent=0,CH4_Kpa=0.0,CH4_Temp=0.0,CH4_per_num = 0 ,CH4_count =0,CH4_per_sum = 0;


void YL_WI_YY_6001_Rcv_One_Char(unsigned char Data)
{

	if(CH4_RcvN<35)
	{
	  CH4_Rcv[CH4_RcvN++]=Data;
	}

	if(Data==0x0A)
	{
		CH4_RcvOK=1;
		CH4_timeout=1;
		YL_WI_YY_6001_Handle();
	}	
}

#if YL_WI_YY_6001
void YL_WI_YY_6001_Handle(void)
{
	unsigned int temp=0;
	unsigned char check=0,j=0;
	unsigned char buf[8];	
	if(CH4_RcvOK==1)// H2s Handle
	{
		if(CH4_Rcv[0]!='+'){goto Err_Handle;}
		
		// check
		for(int i=0;i<25;i++)
		{
		  check=check^CH4_Rcv[i];
		}
		memset(buf,0,sizeof(buf));
		buf[0]=CH4_Rcv[25];
		buf[1]=CH4_Rcv[26];   
		temp=hex_string_to_int(buf);
		
		if(temp!=check){goto Err_Handle;}

		j=0;memset(buf,0,sizeof(buf));	
		for(int i=1;i<7;i++)
		{
		  buf[j++]=CH4_Rcv[i];
		}	
		CH4_percent=str_to_float(buf);

		CH4_per_num = CH4_percent;
		CH4_count++;
		// temp
		j=0;memset(buf,0,sizeof(buf));	
		for(int i=9;i<13;i++)
		{
		  buf[j++]=CH4_Rcv[i];
		}
		CH4_Temp =str_to_float(buf);

		j=0;memset(buf,0,sizeof(buf));	
		for(int i=15;i<21;i++)
		{
		  buf[j++]=CH4_Rcv[i];
		}	
		CH4_Kpa=str_to_float(buf);	
			
//		printf("CH4_percent");
		CH4_RcvOK=0;
		CH4_RcvN=0; 
		memset(CH4_Rcv,0,sizeof(CH4_Rcv));	
		return;		
		
		
		Err_Handle:
		CH4_RcvOK=0;
		CH4_RcvN=0; 
		memset(CH4_Rcv,0,sizeof(CH4_Rcv));

	}	
}

#else
void YL_WI_YY_6001_Handle(void)
{
	unsigned int temp=0;
	unsigned char check=0,j=0;
	unsigned char buf[8];	
	float lel = 0.0;
	if(CH4_RcvOK==1)// H2s Handle
	{
		if(CH4_Rcv[0]!='+'){goto Err_Handle;}
		
		// check
		for(int i=0;i<17;i++)
		{
		  check=check^CH4_Rcv[i];
		}
		memset(buf,0,sizeof(buf));
		buf[0]=CH4_Rcv[17];
		buf[1]=CH4_Rcv[18];   
		temp=hex_string_to_int(buf);
		
		if(temp!=check){goto Err_Handle;}

		j=0;memset(buf,0,sizeof(buf));	
		for(int i=1;i<7;i++)
		{
		  buf[j++]=CH4_Rcv[i];
		}	
		CH4_percent=str_to_float(buf);
//		printf("CH4_percent=%.2f\n\r",CH4_percent);

//		stu_sensor.usCH4 = (unsigned short)(CH4_percent + 0.5);

		lel = CH4_percent/5.0 * 100.0;
		stu_sensor.usCH4 = (unsigned short)lel;

//		if(CH4_START==0)
//		{
//			if(CH4_percent>0)
//			{
//				CH4_per_num = CH4_percent;
//			}
//			if(CH4_percent==0)
//			{
//				CH4_percent = CH4_per_num;
//			}
//			if(Factory_mode==0)
//			{
//				CH4_per_sum = CH4_percent;
//			}
//			printf("CH4_STARTCH4_percent=%.2f\n\r",CH4_percent);
//		}
//		else
//		{
//			if(CH4_percent>0)
//			{
//				CH4_per_sum = CH4_percent;
//			}
//			if(CH4_percent==0)
//			{
//				CH4_per_sum = CH4_per_num;
//			}
//			printf("CH4_STARTCH4_per_sum=%.2f\n\r",CH4_per_sum);
//
//		}

//		CH4_per_sum += 	CH4_percent;
//		CH4_count++;
		// temp
		j=0;memset(buf,0,sizeof(buf));	
		for(int i=9;i<13;i++)
		{
		  buf[j++]=CH4_Rcv[i];
		}
		CH4_Temp =str_to_float(buf);
		
		CH4_RcvOK=0;
		CH4_RcvN=0; 
		memset(CH4_Rcv,0,sizeof(CH4_Rcv));	
			
		return;		
		
		
		Err_Handle:
//		CH4_percent = 0;
		CH4_RcvOK=0;
		CH4_RcvN=0; 
		memset(CH4_Rcv,0,sizeof(CH4_Rcv));

	}	
}
#endif

// 将BCD格式转换为十进制数
int BCDToDecimal(int bcd) {
    int decimal = 0;
    decimal += (bcd/10)*16;
    decimal += (bcd% 10);
    return decimal;
}

float str_to_float(const unsigned char *str) {
    float result = 0.0f;
    float decimal_factor = 1.0f;
    bool is_negative = false;
    bool is_decimal = false;

    // Skip leading whitespaces
    while (isspace(*str)) {
        str++;
    }

    // Handle optional sign
    if (*str == '-' || *str == '+') {
        if (*str == '-') {
            is_negative = true;
        }
        str++;
    }

    // Convert the integral and fractional parts
    while (*str) {
        if (isdigit(*str)) {
            if (is_decimal) {
                decimal_factor *= 0.1f;
                result += (*str - '0') * decimal_factor;
            } else {
                result = result * 10.0f + (*str - '0');
            }
        } else if (*str == '.') {
            if (is_decimal) {
                // Multiple decimal points found, invalid input
                break;
            }
            is_decimal = true;
        } else {
            // Invalid character found, stop parsing
            break;
        }
        str++;
    }

    if (is_negative) {
        result = -result;
    }

    return result;
}

//字符串转整形
unsigned int hex_string_to_int(const unsigned char *hex_str) {
	unsigned int result = 0;
	size_t len = strlen((const char*)hex_str);

	// 遍历字符串中的每个字符
	for (size_t i = 0; i < len; ++i) {
		char c = hex_str[i];

		// 将大写字母转换为小写字母（可选，但为了统一处理）
		c = tolower(c);

		// 检查字符是否为有效的十六进制数字
		if (c >= '0' && c <= '9') {
			result = (result << 4) + (c - '0');
		} else if (c >= 'a' && c <= 'f') {
			result = (result << 4) + (c - 'a' + 10);
		} else {
			// 如果字符不是有效的十六进制数字，则返回错误或设置一个标志
			fprintf(stderr, "Invalid hexadecimal digit: %c\n", c);
			return 0; // 或你可以设置一个错误代码
		}
	}
	return result;
}


double round_to_two_decimal_places(float number) 
{
	// 使用double进行乘法，以减少精度损失  
    double temp = number * 100.0;  
    // 直接截断小数部分，因为(long)转换会去掉小数部分  
    long truncated = (long)temp; 
	double res=(truncated)/100.0;
	
  return res;
}

double Get_CH4_percent()
{

	if(CH4_timeout!=1)
	{
		CH4_percent = 99999;
	}
	else
	{
		CH4_percent = CH4_per_num/CH4_count;
//		if(CH4_percent == 0)
//		{
//			CH4_percent = (rand() % 10 + 2)/100.00;
//		}
	}
  return round_to_two_decimal_places(CH4_percent);
}

double Get_CH4_Kpa()
{
	#if YL_WI_YY_6001
	if(CH4_timeout!=1)
	{
		CH4_Kpa = 99999;
	}
	#else
		CH4_Kpa = 0;
	#endif
	if(MOTOR_flag==1)
	{
		CH4_Kpa =  1;//气压
	}
	
  return round_to_two_decimal_places(CH4_Kpa);
}

double Get_CH4_Temp()
{
	if(CH4_timeout!=1)
	{
		CH4_Temp = 99999;
	}

  return round_to_two_decimal_places(CH4_Temp);
}



unsigned int Get_CH4(void)
{
	if(CH4_timeout!=1||Factory_mode ==0)
	{
		return 0xFFFF;
	}
	else
	{
		if(MOTOR_START==1)
		{
//			printf("CH4_per_sum=%.2f\n\r",CH4_per_sum);
			return CH4_per_sum*100;
		}
		else
		{
//			CH4_percent = CH4_per_sum/CH4_count;
		 	printf("CH4=%.2f,\n\r",CH4_percent);
//			CH4_per_sum =0;
//			CH4_count =0;
			return CH4_percent*100;
		}
		
	}
}


unsigned int Get_Temp(void)
{
	if(CH4_timeout!=1)
	{
		CH4_Temp = 0xFFFF;
	}
	else
	{
		return CH4_Temp * 100;
	}
	
	return CH4_percent;
}

