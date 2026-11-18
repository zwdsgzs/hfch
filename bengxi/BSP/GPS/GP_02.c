#include "GP_02.h"

extern _Para Para,ParaTemp;
char GP_02_RcvBuff[210];
unsigned char GP_02_RcvN=0;
unsigned char GP_02_RcvOK=0,GPSOK_Flag=0;
double latitude=0.00, longitude=0.00;
bool gps_ok = false;

void GP_02_Rcv_One_Char(unsigned char u16Data)
{
//	static unsigned char buf_Head[3];
		
	  if(u16Data=='$')
		{
		  GP_02_RcvN=0;
		}
		if(GP_02_RcvN<200)
		{
		  GP_02_RcvBuff[GP_02_RcvN++]=u16Data;
		}
		if(u16Data==0x0A)
		{
			if (containsGNGLL(GP_02_RcvBuff))
			{
				GPSOK_Flag=0;
				int result = parseAndValidateGNGLL(GP_02_RcvBuff, &latitude, &longitude);
//						printf("\n\n--------GPS:%f,%f",latitude,longitude);
								
				if (latitude >3.0&&longitude>3.0) 
				{
				  GPSOK_Flag=1;

				  stu_sensor.lon = dmm_to_dd(longitude);
				  stu_sensor.lat = dmm_to_dd(latitude);
				  gps_ok = true;
					//printf("---GPRS--OK---\n\n");
				}
				else
				{
					GPSOK_Flag=0;
					//printf("---GPRS--Fail---\n\n");
				}
			}
		}
}


// Function to parse the NMEA GNGLL sentence and validate the length of latitude and longitude
int parseAndValidateGNGLL( char *nmeaSentence, double *latitude, double *longitude) {
    char buffer[100]; // Buffer to hold a copy of the input string
    char lat[20], lon[20];
//	printf("gps----%s----\n",GP_02_RcvBuff);
	  strncpy(buffer, nmeaSentence, sizeof(buffer) - 1); // Copy the input string to buffer
    buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination

	
		char *start = buffer;
	  char *end;
	  char index = 0;
	  // Loop through the string and manually split by commas
	  while ((end = strchr(start, ',')) != NULL) {
		// Temporarily null-terminate the current token
		*end = '\0';
		//printf("Token: %s,index=%d\n", start, index);
		if(index==1)
		{
		  strcpy(lat, start);
		}
		if(index==3)
		{
		  strcpy(lon, start);
		}		
		// Move start to the character after the current comma
		start = end + 1;
		index++;
	}

    // Validate the length of latitude and longitude
    if (strlen(lat) > 4 && strlen(lon) > 4) {
			
			*latitude=str_to_float((unsigned char*)lat);
			*longitude=str_to_float((unsigned char*)lon);
		GP_02_RcvN =0;
		memset(GP_02_RcvBuff,0,sizeof(GP_02_RcvBuff));
        return 1; // Success
    } else {
        return 0; // Error: length of latitude or longitude is not greater than 4
    }
}

// Function to check if a string contains the substring "$GNGLL"
unsigned char containsGNGLL(const char *str) {
    // Use strstr to find the substring "$GNGLL" in the given string
    return strstr(str, "$GNGLL") != NULL;
}


double Get_latitude(void)
{
//	if(latitude==0)
//	{
////		printf("%.6f",Para.latitude);
//		latitude = Para.latitude;
//	}
//	else 
//	{
//		Para.latitude = latitude;
//		ParaWrite();
//	}
 return latitude;
}

double Get_longitude(void)
{
//	if(longitude==0)
//	{
////		printf("%.6f",Para.longitude);
//		longitude = Para.longitude;
//	}
//	else 
//	{
//		Para.longitude = longitude;
//		ParaWrite();
//	}
 return longitude;
}

unsigned char  Get_GPSOK_Flag(void)
{
 return GPSOK_Flag;
}

//度分格式转成小数点格式
double dmm_to_dd(double dmm)
{
    int degrees = (int)(dmm / 100);     // 不管是2位还是3位度，除以100取整都正确
    double minutes = dmm - degrees * 100;
    return degrees + minutes / 60.0;
}


