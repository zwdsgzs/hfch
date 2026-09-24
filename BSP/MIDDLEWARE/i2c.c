#include "i2c.h"

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void I2C1_Init(void) //100KHz
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /* I2C1 clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();
  /**I2C1 GPIO Configuration
  PB6     ------> I2C1_SCL
  PB7     ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* I2C1_Init */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00503D58;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  向指定设备的指定寄存器写入一个字节
  * @param  DevAddress: I2C设备地址 (7位地址即可，函数内部会处理)
  * @param  RegAddress: 寄存器地址
  * @param  Data: 要写入的数据
  * @retval HAL状态
  */
HAL_StatusTypeDef I2C_Write_Byte(uint8_t DevAddress, uint8_t RegAddress, uint8_t Data)
{
    // 注意：HAL库函数通常需要7位地址左移1位，或者直接使用8位地址
    // 这里假设传入的是7位地址，所以 << 1
    return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(DevAddress << 1), RegAddress, I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_TIMEOUT);
}

/**
  * @brief  从指定设备的指定寄存器读取一个字节
  * @param  DevAddress: I2C设备地址 (7位地址)
  * @param  RegAddress: 寄存器地址
  * @param  pData: 存放读取数据的指针
  * @retval HAL状态
  */
HAL_StatusTypeDef I2C_Read_Byte(uint8_t DevAddress, uint8_t RegAddress, uint8_t *pData)
{
    return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(DevAddress << 1), RegAddress, I2C_MEMADD_SIZE_8BIT, pData, 1, I2C_TIMEOUT);
}

/**
  * @brief  从指定设备的指定寄存器读取多个字节 (例如读取传感器数据)
  * @param  DevAddress: I2C设备地址 (7位地址)
  * @param  RegAddress: 寄存器地址
  * @param  pData: 数据缓冲区
  * @param  Size: 读取长度
  * @retval HAL状态
  */
HAL_StatusTypeDef I2C_Read_Buffer(uint8_t DevAddress, uint8_t RegAddress, uint8_t *pData, uint16_t Size)
{
    return HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(DevAddress << 1), RegAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, I2C_TIMEOUT);
}

/**
  * @brief  向指定设备的指定寄存器连续写入多个字节
  * @param  DevAddress: I2C设备地址 (7位地址)
  * @param  RegAddress: 起始寄存器地址
  * @param  pData: 要写入的数据缓冲区
  * @param  Size: 写入长度
  * @retval HAL状态
  */
HAL_StatusTypeDef I2C_Write_Buffer(uint8_t DevAddress, uint8_t RegAddress, uint8_t *pData, uint16_t Size)
{
    return HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(DevAddress << 1), RegAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, I2C_TIMEOUT);
}

