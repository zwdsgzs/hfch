#include "eeprom.h"

#define flashid		2
#define flashidinit		0

flshDataDef stu_flash = {0};
flshDataDef stu_flashjson = {0};

// EEPROM 操作期间，必须保证没有其他中断尝试访问 NVM，否则可能导致程序卡死。通过关中断保护
/*-------------------------------------------------*/
/*函数名：内部eeprom写功能                          */
/*参  数：Address：写入地址                        */
/*参  数：wData：写入数据缓冲区                     */
/*参  数：len：写入数据总长                         */
/*-------------------------------------------------*/
HAL_StatusTypeDef EEPROM_WriteData(uint32_t Address, uint32_t *wData, uint32_t len)
{
    uint32_t i;
    HAL_StatusTypeDef status = HAL_OK;

    /* 参数检查：地址必须在 EEPROM 范围内，且 4 字节对齐 */
    if (Address < DATA_EEPROM_START_ADDR || 
        (Address + len * 4) > DATA_EEPROM_END_ADDR ||
        (Address & 0x3) != 0) {
        return HAL_ERROR;
    }

	/* 解锁 */
	if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK) 
	{
		return HAL_ERROR;
	}
	
	/* 清掉上电残留的 OPTVERR / EOP / 各种 ERROR 标志 */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP      |
						   FLASH_FLAG_WRPERR   |
						   FLASH_FLAG_PGAERR   |
						   FLASH_FLAG_SIZERR   |
						   FLASH_FLAG_OPTVERR  |
						   FLASH_FLAG_RDERR    |
						   FLASH_FLAG_FWWERR   |
						   FLASH_FLAG_NOTZEROERR);

	/* 1) 先按双字粒度擦除整个待写区间 */
	for (i = 0U; i < len; i += 2U) {
		if (HAL_FLASHEx_DATAEEPROM_Erase(Address + i * 4U) != HAL_OK) {
			status = HAL_ERROR;
			HAL_Delay(1);
			break;
		}
	}
	if (status != HAL_OK) return status;

	/* 2) 再按字粒度写入 */
	for (i = 0U; i < len; i++) {
		if (HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,
										   Address + i * 4U,
										   wData[i]) != HAL_OK) {
			status = HAL_ERROR;
			break;
		}
	}

	/* 回读校验（整段） */
    for (i = 0U; i < len; i++) {
        if (*(__IO uint32_t *)(Address + i * 4U) != wData[i]) {
            status = HAL_ERROR;
            break;
        }
    }

	/* 上锁 */
    HAL_FLASHEx_DATAEEPROM_Lock();

    return status;
}

/*-------------------------------------------------*/
/*函数名：内部eeprom读功能                        */
/*参  数：Address：读取地址                        */
/*参  数：rData：保存数据缓冲区                     */
/*参  数：len：读取数据总长                         */
/*返回值：无                                       */
/*-------------------------------------------------*/
HAL_StatusTypeDef EEPROM_ReadData(uint32_t Address, uint32_t *rData, uint32_t len)
{
    uint32_t i;

    /* 参数检查 */
    if (Address < DATA_EEPROM_START_ADDR || 
        (Address + len * 4) > DATA_EEPROM_END_ADDR) {
        return HAL_ERROR;
    }

    /* 读取操作可以直接通过指针访问，不需要解锁和中断保护[citation:10] */
    for (i = 0; i < len; i++) {
        rData[i] = *(__IO uint32_t*)(Address + i * 4);
    }

    return HAL_OK;
}


/*-------------------------------------------------*/
/*函数名：带有校验操作的内部eeprom写功能                          */
/*参  数：Address：写入地址                        */
/*参  数：wData：写入数据缓冲区                     */
/*参  数：len：写入数据总长                         */
/*返回值：无                                       */
/*-------------------------------------------------*/
HAL_StatusTypeDef EEPROM_Write_Verify_CHECK(uint32_t Address, uint32_t *wData, uint32_t len)
{
    uint32_t buff[64]; // 假设 len 不会超过 64
    uint32_t retry;

    if (len > 64) return HAL_ERROR;

    for (retry = 0; retry < 3; retry++) {
        if (EEPROM_WriteData(Address, wData, len) != HAL_OK) {
            continue;
        }

        if (EEPROM_ReadData(Address, buff, len) != HAL_OK) {
            continue;
        }

        if (memcmp(wData, buff, len * 4) == 0) {
            return HAL_OK;
        }
    }

    return HAL_ERROR;
}

/* ============================================================
 *                      从 EEPROM 读取
 * ============================================================ */

int flashReadFromEEPROM(void)
{
    uint32_t magic = 0;
    uint32_t word_len;
    uint32_t *p;

#if (flashidinit == 1)
	flashDataInit();
    flashWriteToEEPROM();
	HAL_Delay(1000);
#endif

    //  先读魔数，判断 EEPROM 是否有效 
    if (EEPROM_ReadData(PARA_MAGIC_ADDR, &magic, 1) != HAL_OK) {
        return -1;
    }

    if (magic != PARA_MAGIC_VALUE) 
	{
        // 魔数不匹配，说明 EEPROM 没写过或数据无效 
		flashDataInit();
        flashWriteToEEPROM();
		HAL_Delay(1000);
        return -1;
    }

    //  魔数正确，读参数
    word_len = sizeof(flshDataDef) / 4;
    p = (uint32_t *)&stu_flash;

    if (EEPROM_ReadData(PARA_DATA_ADDR, p, word_len) != HAL_OK) {
        return -1;
    }

    //  确保字符串以 '\0' 结尾（防止 EEPROM 里的脏数据导致越界）
    stu_flash.productKey[sizeof(stu_flash.productKey) - 1]     = '\0';
    stu_flash.deviceName[sizeof(stu_flash.deviceName) - 1]     = '\0';
    stu_flash.deviceSecret[sizeof(stu_flash.deviceSecret) - 1] = '\0';
    stu_flash.mqttBroker[sizeof(stu_flash.mqttBroker) - 1]     = '\0';

    return 0;
}

/* ============================================================
 *                      写入 EEPROM
 * ============================================================ */

int flashWriteToEEPROM(void)
{
    /* 拼成一个大缓冲区：4 字节魔数 + 200 字节结构体 */
    uint8_t buf[4 + sizeof(flshDataDef)];

    uint32_t magic = PARA_MAGIC_VALUE;
    memcpy(buf, &magic, 4);                                    /* 前 4 字节：魔数 */
    memcpy(buf + 4, &stu_flash, sizeof(flshDataDef));          /* 后面：数据 */

    /* 一次调用：从 0x08080000 开始，写 4+200 = 204 字节 */
    uint32_t total_words = (4 + sizeof(flshDataDef)) / 4;      /* 51 个字 */
	
	if (EEPROM_WriteData(PARA_MAGIC_ADDR, (uint32_t *)buf, total_words) != HAL_OK) {
        return -1;
    }

    return 0;
}

void flashDataInit(void)
{ 
#if (flashid == 1)
	strcpy(stu_flash.deviceName,"dev_0001");//经测试，纯数字id最多8位，带英文id最多9位
	strcpy(stu_flash.deviceSecret,"dWg0QXhnQW5TVWEzVFNPelBwQkFTQWVGNHZmaVF3aXk=");
#elif (flashid == 2)
	strcpy(stu_flash.deviceName,"dev_0002");//经测试，纯数字id最多8位，带英文id最多9位
	strcpy(stu_flash.deviceSecret,"UTV1MmFESWlnYUdibWFQTFZYUkVFbDJOYzNQV2IzajM=");
#endif

	strcpy(stu_flash.mqttBroker,"z8k09jGpX5.mqtts.acc.cmcconenet.cn");
	strcpy(stu_flash.productKey,"z8k09jGpX5");

	stu_flash.fAngle = 30.0;
	stu_flash.updateIntervalmins = 24*60/2;//计算出休眠时间
}


