/* DMOSDK - DecaWave Module Open Software Development Kit
 * Copyright (C) 2018-2019 IRIT-RMESS
 *
 * This file is part of DMOSDK.
 *
 * DMOSDK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DMOSDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DMOSDK.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Simple Deca Driver FreeRTOS example application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to send a counter value over UWB.
 *
 */
#include "FreeRTOS.h"
#include "task.h"

#include "deca_device_api.h"
#include "deca_regs.h"
#include "port_platform.h"

static dwt_config_t range_optimized_config = {
    4,                /* Channel number. */
    DWT_PRF_64M,      /* Pulse repetition frequency. */
    DWT_PLEN_4096,    /* Preamble length. Used in TX only. */
    DWT_PAC64,        /* Preamble acquisition chunk size. Used in RX only. */
    17,               /* TX preamble code. Used in TX only. */
    17,               /* RX preamble code. Used in RX only. */
    0,                /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_110K,      /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    4161     /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

/**
 * @brief Initializes the DW1000 radio
 */
void dwm1000_init(void)
{
  reset_DW1000(); 

  /* Set SPI clock to 2MHz */
  port_set_dw1000_slowrate();     
  
  if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
  {
    while (1);
  }

  // Set SPI to 8MHz clock
  port_set_dw1000_fastrate();

  dwt_configure(&range_optimized_config);

  dwt_setrxantennadelay(RX_ANT_DLY);
  dwt_settxantennadelay(TX_ANT_DLY);
}

TaskHandle_t  sender_task_handle;
void sender_task_function (void * pvParameter)
{
    uint32_t counter = 0;
    uint8_t buffer[sizeof(counter)+2];
    while(1)
    {
        TickType_t start_tick = xTaskGetTickCount();
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
        for (int i = 0; i < sizeof(counter); i++)
        {
            buffer[i] = *(((uint8_t *)&counter)+ i);
        }
        dwt_writetxdata(sizeof(buffer), buffer, 0); /* Zero offset in TX buffer. */
        dwt_writetxfctrl(sizeof(buffer),0, 0); /* Zero offset in TX buffer, no ranging. */
        dwt_starttx(DWT_START_TX_IMMEDIATE);
        vTaskDelayUntil(&start_tick, 1002);
        counter++;
    }
}

int main(void)
{
  dwm1000_init();

  xTaskCreate(sender_task_function, "S", configMINIMAL_STACK_SIZE + 50, NULL, 1, &sender_task_handle);

  vTaskStartScheduler();
  while(1);
}

