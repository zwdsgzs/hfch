/**
 *******************************************************************************
 * @file  hc32f4xx_conf.h
 * @brief This file contains HC32 Series Device Driver Library usage management.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2023-05-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2023, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#ifndef __HC32F4XX_CONF_H__
#define __HC32F4XX_CONF_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/

/**
 * @brief This is the list of modules to be used in the Device Driver Library.
 * Select the modules you need to use to DDL_ON.
 * @note LL_ICG_ENABLE must be turned on(DDL_ON) to ensure that the chip works
 * properly.
 * @note LL_UTILITY_ENABLE must be turned on(DDL_ON) if using Device Driver
 * Library.
 * @note LL_PRINT_ENABLE must be turned on(DDL_ON) if using printf function.
 */
#define LL_ICG_ENABLE                               (DDL_ON)		//	初始化配置（ICG）
#define LL_UTILITY_ENABLE                           (DDL_ON)		//	 DDL 工具函数
#define LL_PRINT_ENABLE                             (DDL_OFF)		//	printf 重定向支持

#define LL_ADC_ENABLE                               (DDL_ON)		//  12 位 ADC                
#define LL_AES_ENABLE                               (DDL_OFF)       //  AES 加解密                
#define LL_AOS_ENABLE                               (DDL_OFF)       //  自动运行系统（外设联动     
#define LL_CLK_ENABLE                               (DDL_ON)        //  时钟控制 CMU              
#define LL_CMP_ENABLE                               (DDL_OFF)       //  电压比较器               
#define LL_CRC_ENABLE                               (DDL_OFF)       //  硬件 CRC                 
#define LL_CTC_ENABLE                               (DDL_OFF)       //  内部时钟校准              
#define LL_DAC_ENABLE                               (DDL_OFF)       //  12 位 DAC                
#define LL_DBGC_ENABLE                              (DDL_OFF)       //  调试控制器              
#define LL_DCU_ENABLE                               (DDL_OFF)       //  数据计算单元              
#define LL_DMA_ENABLE                               (DDL_OFF)       //  DMA 控制器                
#define LL_EFM_ENABLE                               (DDL_ON)        //  嵌入式 Flash（存参数）     
#define LL_EMB_ENABLE                               (DDL_OFF)       //  紧急刹车（电机 PWM）      
#define LL_EVENT_PORT_ENABLE                        (DDL_OFF)       //  事件端口                  
#define LL_FCG_ENABLE                               (DDL_ON)        //  外设时钟门控              
#define LL_FCM_ENABLE                               (DDL_OFF)       //  时钟频率监测              
#define LL_GPIO_ENABLE                              (DDL_ON)        //  GPIO                     
#define LL_HASH_ENABLE                              (DDL_OFF)       //  SHA-256                  
#define LL_I2C_ENABLE                               (DDL_OFF)       //  I2C                      
#define LL_INTERRUPTS_ENABLE                        (DDL_ON)        //  中断控制器 INTC           
#define LL_KEYSCAN_ENABLE                           (DDL_OFF)       //  键盘扫描               
#define LL_MCAN_ENABLE                              (DDL_OFF)       //  CAN FD                   
#define LL_MPU_ENABLE                               (DDL_OFF)       //  存储保护单元       
#define LL_PWC_ENABLE                               (DDL_ON)        //  电源控制（STOP 模式） 
#define LL_QSPI_ENABLE                              (DDL_OFF)       //  QSPI                     
#define LL_RMU_ENABLE                               (DDL_ON)        //  复位管理                  
#define LL_RTC_ENABLE                               (DDL_ON)        //  实时时钟 / 闹钟           
#define LL_SMC_ENABLE                               (DDL_OFF)       //  存储控制器           
#define LL_SPI_ENABLE                               (DDL_OFF)       //  SPI                      
#define LL_SRAM_ENABLE                              (DDL_ON)        //  内置 SRAM 管理            
#define LL_SWDT_ENABLE                              (DDL_OFF)       //  专用看门狗                
#define LL_TMR0_ENABLE                              (DDL_OFF)       //  基础定时器 Timer0    
#define LL_TMR4_ENABLE                              (DDL_OFF)       //  电机 PWM 定时器        
#define LL_TMR6_ENABLE                              (DDL_OFF)       //  高级 PWM 定时器         
#define LL_TMRA_ENABLE                              (DDL_OFF)       //  通用定时器 TimerA         
#define LL_TRNG_ENABLE                              (DDL_OFF)       //  真随机数                  
#define LL_USART_ENABLE                             (DDL_ON)        //  串口                 
#define LL_WDT_ENABLE                               (DDL_OFF)       //  通用看门狗                

/**
 * @brief The following is a list of currently supported BSP boards.
 */
#define BSP_EV_HC32F448_LQFP80                      (9U)

/**
 * @brief The macro BSP_EV_HC32F4XX is used to specify the BSP board currently
 * in use.
 * The value should be set to one of the list of currently supported BSP boards.
 * @note  If there is no supported BSP board or the BSP function is not used,
 * the value needs to be set to 0U.
 */
#define BSP_EV_HC32F4XX                             (0U)

/**
 * @brief This is the list of BSP components to be used.
 * Select the components you need to use to DDL_ON.
 */
#define BSP_24CXX_ENABLE                            (DDL_OFF)
#define BSP_GT9XX_ENABLE                            (DDL_OFF)
#define BSP_IS61LV6416_ENABLE                       (DDL_OFF)
#define BSP_NT35510_ENABLE                          (DDL_OFF)
#define BSP_TCA9539_ENABLE                          (DDL_OFF)
#define BSP_W25QXX_ENABLE                           (DDL_OFF)
#define BSP_INT_KEY_ENABLE                          (DDL_OFF)

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __HC32F4XX_CONF_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

