#include "main.h"

int cnttttt = 0;
void appNetTask(void *argument)
{
	osDelay(100);
    PowerOn_4G();
	
	EC800M_Poll_Rx();
//	Init_4G();
#if (DEBUG_UART5 == 1)
	printf("start");
#endif
	EC800M_Poll_Rx();
			PowerOff_4G();
	EC800M_Poll_Rx();
			PowerOn_4G();
	for(;;)
	{
		if (stu_4G.InitFlag == 0)//初始化未成功，或者休眠后重连失败
		{
			PowerOff_4G();
			EC800M_Poll_Rx();
			PowerOn_4G();
			EC800M_Poll_Rx();
			Init_4G();
			EC800M_Poll_Rx();
			osDelay(3000);
			
		}
//		else{
//			switch (stu_4G.process)
//			{
//			case 1:
//				if (stu_flag.bAccFlag)//盖子打开
//				{
//					Mq_Pub_open();
//					stu_flag.bAccFlag = false;
//				}
//				break;
//			case 2:
//				Mq_Sub_shadow();
//				stu_4G.process = 3;
//				osDelay(2000);
//				break;
//			case 3:
//				osDelay(2000);
//				Mq_Pub_P();
//				break;
//			default:break;
//			}

//			EC800M_Poll_Rx();
//		}

		osDelay(1000);
	}
}


