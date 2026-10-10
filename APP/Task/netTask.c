#include "main.h"


void appNetTask(void *argument)
{
	osDelay(100);
    PowerOn_4G();
	osDelay(5000);//等待模块启动（建议 3~5 秒）
//                                                                                                                                                                                     

	printf("start");

//	if (stu_4G.InitFlag) 
//	{
//        stu_4G.get_frist_Mq_flg = 2;
//        Mq_Sub_shadow();
//        stu_4G.get_frist_Mq_flg = 1;
//    }

	for(;;)
	{
//		/* 处理 URC（放在 AT_Send 之外的间隙） */
//		EC800M_Poll_Rx();

//		/* 检查 MQTT 断线 */
//		if (strstr(stu_4G.From4G, "+QMTSTAT")) {
//			memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
//			stu_4G.From4G_N = 0;
//			reConnect_4G();
//		}

//		//上报数据
//		if (stu_4G.reported == 1)
//		{
//			Mq_Pub_P();
//			stu_4G.reported = 2;
//		}
		EC800M_Poll_Rx();
		osDelay(50);
	}
}


