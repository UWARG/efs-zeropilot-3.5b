/*
 * user_diskio_sdmmc.c
 *
 *  Created on: Feb 28, 2025
 *      Author: arigu
 */

#include "user_diskio_sdmmc.h"

#include "stm32l5xx_hal.h"
#include "utils.h"

#ifdef SDMMC_INTERFACE

extern SD_HandleTypeDef hsd1;

inline DSTATUS USER_SDMMC_initialize (BYTE lun) {
  DRESULT res = RES_ERROR;

  if (Stat & STA_NODISK) return Stat;

  res = HAL_SD_Init(&hsd1);
  if (res == RES_OK) {
    Stat &= ~STA_NOINIT;
  }

  return res;
}

inline DSTATUS USER_SDMMC_status (BYTE lun) {
  return Stat;
}

inline DRESULT USER_SDMMC_read (BYTE lun, BYTE *buff, DWORD sector, UINT count) {
  DRESULT res = RES_ERROR;

  if (osKernelGetState() == osKernelRunning) {
    HAL_SD_ReadBlocks_DMA(&hsd1, buff, sector, count);

    uint32_t start = osKernelGetTickCount();
    uint32_t ticks = start;
    while (!readStatus && (osKernelGetTickCount() - start) < timeToTicks(SD_TIMEOUT)) {
      ticks += timeToTicks(10);
      osDelayUntil(ticks);
    }
  } else {
    HAL_SD_ReadBlocks(&hsd1, buff, sector, count, HAL_MAX_DELAY);
    readStatus = 1;
  }

  if (readStatus) {
    readStatus = 0;
    res = RES_OK;
  }

  return res;
}

inline DRESULT USER_SDMMC_write (BYTE lun, const BYTE *buff, DWORD sector, UINT count) {
  DRESULT res = RES_ERROR;

  if (osKernelGetState() == osKernelRunning) {
    HAL_SD_WriteBlocks_DMA(&hsd1, buff, sector, count);

    uint32_t start = osKernelGetTickCount();
    uint32_t ticks = start;
    while (!writeStatus && (osKernelGetTickCount() - start) < timeToTicks(SD_TIMEOUT)) {
      ticks += timeToTicks(10);
      osDelayUntil(ticks);
    }
  } else {
    HAL_SD_WriteBlocks(&hsd1, buff, sector, count, HAL_MAX_DELAY);
    writeStatus = 1;
  }

  if (writeStatus) {
    writeStatus = 0;
    res = RES_OK;
  }

  return res;
}

inline DRESULT USER_SDMMC_ioctl (BYTE lun, BYTE cmd, void *buff) {
  DRESULT res = RES_ERROR;
  HAL_SD_CardInfoTypeDef CardInfo;

  switch (cmd)
  {
  case CTRL_SYNC :
    res = RES_OK;
    break;

  case GET_SECTOR_COUNT :
    HAL_SD_GetCardInfo(&hsd1, &CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockNbr;
    res = RES_OK;
    break;

  case GET_SECTOR_SIZE :
    HAL_SD_GetCardInfo(&hsd1, &CardInfo);
    *(WORD*)buff = CardInfo.LogBlockSize;
    res = RES_OK;
    break;

  case GET_BLOCK_SIZE :
    HAL_SD_GetCardInfo(&hsd1, &CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
    res = RES_OK;
    break;

  default :
    res = RES_PARERR;
  }

  return res;
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
    if (hsd->Instance == SDMMC1) {
      writeStatus = 1;
    }
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
  if (hsd->Instance == SDMMC1) {
    readStatus = 1;
  }
}

#endif
