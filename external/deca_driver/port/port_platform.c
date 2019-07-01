/*! ----------------------------------------------------------------------------
 * @file    port_platform.c
 * @brief   HW specific definitions and functions for portability
 *
 * @attention
 *
 * Copyright 2016 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "port_platform.h"
#include "deca_device_api.h"

/****************************************************************************//**
 *
 *                              APP global variables
 *
 *******************************************************************************/


/****************************************************************************//**
 *
 *                  Port private variables and function prototypes
 *
 *******************************************************************************/
static volatile uint32_t signalResetDone;

/****************************************************************************//**
 *
 *                              Time section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *                              END OF Time section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *                              Configuration section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *                          End of configuration section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *                          DW1000 port section
 *
 *******************************************************************************/
 // YB : STM HAL based function have to be updated using NRF drivers


/* @fn      setup_DW1000RSTnIRQ
 * @brief   setup the DW_RESET pin mode
 *          0 - output Open collector mode
 *          !0 - input mode with connected EXTI0 IRQ
 * */
void setup_DW1000RSTnIRQ(int enable)
{
}



/* @fn      port_wakeup_dw1000
 * @brief   "slow" waking up of DW1000 using DW_CS only
 * */
void port_wakeup_dw1000(void)
{
}

/* @fn      port_wakeup_dw1000_fast
 * @brief   waking up of DW1000 using DW_CS and DW_RESET pins.
 *          The DW_RESET signalling that the DW1000 is in the INIT state.
 *          the total fast wakeup takes ~2.2ms and depends on crystal startup time
 * */
void port_wakeup_dw1000_fast(void)
{
}

//================================================================================================
int readfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer)
{ uint8 * p1;
  uint32 idatalength=0;

  idatalength= headerLength + readlength;

  uint8 idatabuf[idatalength];
  uint8 itempbuf[idatalength];

  memset(idatabuf, 0, idatalength);
  memset(itempbuf, 0, idatalength);	

  p1=idatabuf;	
  memcpy(p1,headerBuffer, headerLength);

  p1 += headerLength;
  memset(p1,0x00,readlength);

  spi_xfer_done = false;
  nrfx_spi_xfer_desc_t xfer_desc = NRFX_SPI_XFER_TRX(
    idatabuf,
    idatalength,
    itempbuf,
    idatalength
  );
  nrfx_spi_xfer(&spi, &xfer_desc, 0);

  p1=itempbuf + headerLength;

  memcpy(readBuffer, p1, readlength);

  return 0;
} 


int writetospi( uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer)
{
  uint8 * p1;
  uint32 idatalength=0;

  idatalength= headerLength + bodylength;

  uint8 idatabuf[idatalength];
  uint8 itempbuf[idatalength];

  memset(idatabuf, 0, idatalength);
  memset(itempbuf, 0, idatalength);		 
  
  p1=idatabuf;	
  memcpy(p1,headerBuffer, headerLength);
  p1 += headerLength;
  memcpy(p1,bodyBuffer,bodylength);
  
  spi_xfer_done = false;
  nrfx_spi_xfer_desc_t xfer_desc = NRFX_SPI_XFER_TRX(
    idatabuf,
    idatalength,
    itempbuf,
    idatalength
  );
  nrfx_spi_xfer(&spi, &xfer_desc, 0);

  return 0;
} 

//------------------------------other---------------------------

#define NRFX_SPI_DEFAULT_CONFIG_2M(id)                      \
{                                                           \
    .sck_pin      = NRFX_CONCAT_3(SPIM, id, _SCK_PIN),      \
    .mosi_pin     = NRFX_CONCAT_3(SPIM, id, _MOSI_PIN),     \
    .miso_pin     = NRFX_CONCAT_3(SPIM, id, _MISO_PIN),     \
    .ss_pin       = NRFX_CONCAT_3(SPIM, id, _SS_PIN),       \
    .irq_priority = NRFX_CONCAT_3(SPIM, id, _IRQ_PRIORITY), \
    .orc          = 0xFF,                                   \
    .frequency    = NRF_SPI_FREQ_2M,                        \
    .mode         = NRF_SPI_MODE_0,                         \
    .bit_order    = NRF_SPI_BIT_ORDER_MSB_FIRST,            \
}


#define NRFX_SPI_DEFAULT_CONFIG_8M(id)                      \
{                                                           \
    .sck_pin      = NRFX_CONCAT_3(SPIM, id, _SCK_PIN),      \
    .mosi_pin     = NRFX_CONCAT_3(SPIM, id, _MOSI_PIN),     \
    .miso_pin     = NRFX_CONCAT_3(SPIM, id, _MISO_PIN),     \
    .ss_pin       = NRFX_CONCAT_3(SPIM, id, _SS_PIN),       \
    .irq_priority = NRFX_CONCAT_3(SPIM, id, _IRQ_PRIORITY), \
    .orc          = 0xFF,                                   \
    .frequency    = NRF_SPI_FREQ_8M,                        \
    .mode         = NRF_SPI_MODE_0,                         \
    .bit_order    = NRF_SPI_BIT_ORDER_MSB_FIRST,            \
}

/* @fn      reset_DW1000
 * @brief   DW_RESET pin on DW1000 has 2 functions
 *          In general it is output, but it also can be used to reset the digital
 *          part of DW1000 by driving this pin low.
 *          Note, the DW_RESET pin should not be driven high externally.
 * */
void reset_DW1000(void)
{
  nrf_gpio_cfg_output(DW1000_RST);   
  nrf_gpio_pin_clear(DW1000_RST);  
  NRFX_DELAY_US(2000);
  //nrf_gpio_pin_set(DW1000_RST);  
  //NRFX_DELAY_US(50);
  nrf_gpio_cfg_input(DW1000_RST, NRF_GPIO_PIN_NOPULL); 
  NRFX_DELAY_US(2000);
}

/* @fn      port_set_dw1000_slowrate
 * @brief   set 2MHz
 *          n
 * */
void port_set_dw1000_slowrate(void)
{
	nrfx_spi_config_t spi_config = NRFX_SPI_DEFAULT_CONFIG_2M(SPI_INSTANCE);
    nrfx_spi_init(&spi, &spi_config, NULL, NULL);
	NRFX_DELAY_US(2000);	
}

/* @fn      port_set_dw1000_fastrate
 * @brief   set 8MHz
 *         
 * */
void port_set_dw1000_fastrate(void)
{
    nrfx_spi_uninit(&spi);
	nrfx_spi_config_t spi_config = NRFX_SPI_DEFAULT_CONFIG_8M(SPI_INSTANCE);
    nrfx_spi_init(&spi, &spi_config, NULL, NULL);
	NRFX_DELAY_US(2000);	
}


void deca_sleep(unsigned int time_ms)
{
    NRFX_DELAY_US(time_ms * 1000);
}


// currently do nothing
decaIrqStatus_t decamutexon(void)           
{
//	u16 j = (u16)(NVIC->ISER[0] & (1 << 5));

	//	if(j) 
//  {
//		NVIC->ISER[0] &= ~(1 << 5); //disable the external interrupt line
//	}
//	return j ; 

return 0;	
}


// currently do nothing
void decamutexoff(decaIrqStatus_t s)       
{
//	if(j) 

	//	{                 
//		NVIC->ISER[0] |= (1 << 5);;
//	}
	;	
}

/****************************************************************************//**
 *
 *                          End APP port section
 *
 *******************************************************************************/



/****************************************************************************//**
 *
 *                              IRQ section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *                              END OF IRQ section
 *
 *******************************************************************************/

/****************************************************************************//**
 *
 *******************************************************************************/

