/*
 * lcd.c
 *
 *  Created on: 2026. 3. 18.
 *      Author: yougang
 */

#include "spi.h"

#ifdef _USE_HW_SPI

typedef struct
{
  bool is_open;
  bool tx_done;
  void (*func_tx)(void);

  SPI_HandleTypeDef* hspi;
  DMA_HandleTypeDef* hdma_spi_tx;

}spi_tbl_t;

SPI_HandleTypeDef hspi1;

spi_tbl_t spi_tbl[HW_SPI_MAX_CH];

bool spiInit(void)
{
  bool ret=true;
  for(int i=0; i<HW_SPI_MAX_CH; i++)
  {
    spi_tbl[i].is_open=false;
    spi_tbl[i].tx_done=true;
    spi_tbl[i].func_tx = NULL;
  }
  return ret;
}
bool spiBegin(uint8_t ch)
{
  spi_tbl_t* p_spi=&spi_tbl[ch];
  bool ret=false;
  switch(ch)
  {
    case _DEF_SPI1:
      p_spi->hspi          = &hspi1;

      hspi1.Instance = SPI1;
      hspi1.Init.Mode = SPI_MODE_MASTER;
      hspi1.Init.Direction = SPI_DIRECTION_2LINES;
      hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
      hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
      hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
      hspi1.Init.NSS = SPI_NSS_SOFT;
      hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
      hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
      hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
      hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
      hspi1.Init.CRCPolynomial = 10;

      if (HAL_SPI_Init(&hspi1) == HAL_OK)
      {
        p_spi->is_open       = true;
        ret=true;
      }
      break;
  }
  return ret;
}



void spiSetDataMode(uint8_t ch, uint8_t dataMode)
{
  spi_tbl_t* p_spi=&spi_tbl[ch];
  if(ch>=HW_SPI_MAX_CH) return;
  if(p_spi->is_open != true) return;
  switch(ch)
  {
    // CPOL : Clock Polarity, CPHA: Clock Phase
    // CPOL : 0 CPHA : 0
    case SPI_MODE0:
      p_spi->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->hspi->Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(p_spi->hspi);
      break;
      // CPOL : 0 CPHA : 1
    case SPI_MODE1:
      p_spi->hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->hspi->Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(p_spi->hspi);
      break;
      // CPOL : 1 CPHA : 0
    case SPI_MODE2:
      p_spi->hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->hspi->Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(p_spi->hspi);
      break;
      // CPOL : 1 CPHA :1
    case SPI_MODE3:
      p_spi->hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->hspi->Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(p_spi->hspi);
      break;
  }

}




void spiSetBitWidth(uint8_t ch, uint8_t bit_width)
{
  spi_tbl_t* p_spi=&spi_tbl[ch];
  if(ch>=HW_SPI_MAX_CH) return;
  p_spi->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  if(bit_width==16)
  {
    p_spi->hspi->Init.DataSize = SPI_DATASIZE_16BIT;
  }
  HAL_SPI_Init(p_spi->hspi);
}

uint8_t spiTransfer8(uint8_t ch, uint8_t data)
{
  uint8_t rx_data;
  spi_tbl_t* p_spi=&spi_tbl[ch];
  if(p_spi->is_open != true) return 0;

  if(HAL_SPI_TransmitReceive(p_spi->hspi, &data, &rx_data,1, 0xFFFF) == HAL_OK)
  {
    return rx_data;
  }
  return 0;
}


uint16_t spiTransfer16(uint8_t ch, uint16_t tx_16_buf)
{
  uint8_t     rx_8buf[2];
  uint8_t     tx_8buf[2];
  uint16_t    rx_16_buf;

  spi_tbl_t* p_spi=&spi_tbl[ch];
  if(p_spi->is_open != true) return 0;
  if((p_spi->hspi->Init.DataSize == SPI_DATASIZE_8BIT))
  {
    tx_8buf[0] = (uint8_t)((tx_16_buf>>8)&0xFF);
    tx_8buf[1] = (uint8_t)((tx_16_buf>>0)&0xFF);
    HAL_SPI_TransmitReceive(p_spi->hspi, &tx_8buf[0], &rx_8buf[2],2, 0xff);
    rx_16_buf = rx_8buf[0];
    rx_16_buf <<= 8;
    rx_16_buf |= rx_8buf[1];
  }
  else
  {
    HAL_SPI_TransmitReceive(p_spi->hspi, (uint8_t *)&tx_16_buf, (uint8_t *)&rx_16_buf, 1, 0xffff);

  }

  return rx_16_buf;
}

void spiDmaTxStart(uint8_t ch, uint8_t *p_buf, uint32_t length)
{
  spi_tbl_t* p_spi = &spi_tbl[ch];
  if(p_spi->is_open != true) return;
  p_spi->tx_done = false;
  HAL_SPI_Transmit_DMA(p_spi->hspi, p_buf, (uint16_t)length);

}
void spiDmaTxTransfer(uint8_t ch, void *buf, uint32_t length, uint32_t timeout)
{
  uint32_t pre_time;
  spi_tbl_t* p_spi = &spi_tbl[ch];
  if(p_spi->is_open != true) return;
  spiDmaTxStart(ch,(uint8_t*)buf, length);
  p_spi->tx_done= false;
  pre_time=millis();
  while(1)
  {
    if(spiDmaTxIsDone(ch))
    {
      break;
    }
    if(millis()-pre_time>timeout)
    {
      break;
    }
  }
}


bool spiDmaTxIsDone(uint8_t ch)
{
  spi_tbl_t* p_spi = &spi_tbl[ch];
  if(p_spi->is_open != true) p_spi->tx_done = true;
  return p_spi->tx_done;
}


void spiAttachTxInterrupt(uint8_t ch, void (*func)())
{
  spi_tbl_t* p_spi = &spi_tbl[ch];
  if(p_spi->is_open != true) return;
  p_spi->func_tx=func;

}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  if(hspi->Instance==spi_tbl[_DEF_SPI1].hspi->Instance)
  {
    spi_tbl_t* p_spi = &spi_tbl[_DEF_SPI1];
    p_spi->tx_done = true;
    if(p_spi->func_tx != NULL)
    {
      (*p_spi->func_tx)();
    }
  }
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
     */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN SPI1_MspInit 1 */

    /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */

    /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
     */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* USER CODE BEGIN SPI1_MspDeInit 1 */

    /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#endif
