#include "CRC_SUM.h"

unsigned int crcVal1(unsigned char *pcMess,unsigned int wLen)
{
  long MSBInfo;
  int i,j ;
  unsigned int nCRCData;
  nCRCData = 0xffff;
  for(i = 0; i < wLen;i++)
  {
    pcMess[i]&=0x00ff;
    nCRCData = nCRCData ^ pcMess[i] ;
    for(j= 0 ; j < 8 ;j ++)
      {
        MSBInfo = nCRCData & 0x0001;
        nCRCData = nCRCData >> 1;
        if(MSBInfo != 0 )
          nCRCData = nCRCData ^ 0xa001;
      }
  }
  return nCRCData;
}



unsigned char FucCheckSum(unsigned char *i, unsigned char ln)
{
	unsigned char j, tempq=0;
	i+=1;
	for(j=0; j<(ln-2); j++)
	{
		tempq+=*i;
		i++;
	}
	tempq=(~tempq)+1;
	return(tempq);
}




