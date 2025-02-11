/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sd_diskio.c (for L5)
  * @brief   SD Disk I/O driver
  * @note    To be completed by the user according to the project board in use.
  *          (see templates available in the FW pack, Middlewares\Third_Party\FatFs\src\drivers folder).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"
#include "sd_diskio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SD_DEFAULT_BLOCK_SIZE 512
#define SD_TIMEOUT 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern SD_HandleTypeDef hsd1;
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static volatile uint8_t readStatus = 0;
static volatile uint8_t writeStatus = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
DSTATUS SD_initialize (BYTE);
DSTATUS SD_status (BYTE);
DRESULT SD_read (BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
  DRESULT SD_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT SD_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

const Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read,
#if  _USE_WRITE == 1
  SD_write,
#endif /* _USE_WRITE == 1 */

#if  _USE_IOCTL == 1
  SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_initialize(BYTE lun)
{
  /* USER CODE BEGIN SD_initialize */
  DRESULT res = RES_ERROR;

  if (Stat & STA_NODISK) {
    return Stat;
  }

  res = HAL_SD_Init(&hsd1);
  if (res == RES_OK) {
    Stat &= ~STA_NOINIT;
  }

  return res;
  /* USER CODE END SD_initialize */
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_status(BYTE lun)
{
  /* USER CODE BEGIN SD_status */
  return Stat;
  /* USER CODE END SD_status */
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN SD_read */
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
    HAL_SD_ReadBlocks(&hsd1, buff, sector, count);
    readStatus = 1;
  }

  if (readStatus) {
    readStatus = 0;
    res = RES_OK;
  }

  return res;
  /* USER CODE END SD_read */
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : not used
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  /* USER CODE BEGIN SD_write */
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
    HAL_SD_WriteBlocks(&hsd1, buff, sector, count);
    writeStatus = 1;
  }

  if (writeStatus) {
    writeStatus = 0;
    res = RES_OK;
  }

  return res;
  /* USER CODE END SD_write */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  /* USER CODE BEGIN SD_ioctl */
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
  /* USER CODE END SD_ioctl */
}
#endif /* _USE_IOCTL == 1 */

/* USER CODE BEGIN UserCode */
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
/* USER CODE END UserCode */
