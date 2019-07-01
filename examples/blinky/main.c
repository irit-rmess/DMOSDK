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
 * @brief Blinky FreeRTOS example application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to blink a LED. Adapted from nRF5_SDK's blinky example.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "nrf_gpio.h"

#define LED_PIN 13

#define TASK_DELAY 100

TaskHandle_t  led_toggle_task_handle;   /**< Reference to LED toggling FreeRTOS task. */

/**@brief LED task entry function.
 *
 * @param[in] pvParameter Pointer that will be used as the parameter for the task.
 */
static void led_toggle_task_function (void * pvParameter)
{
    while (true)
    {
        nrf_gpio_pin_toggle(LED_PIN);

        /* Delay a task for a given number of ticks */
        vTaskDelay(TASK_DELAY);

        /* Tasks must be implemented to never return... */
    }
}

int main(void)
{
    /* Configure LED-pins as outputs */
    nrf_gpio_cfg_output(LED_PIN);

    /* Create task for led blinking with priority set to 1 */
    xTaskCreate(led_toggle_task_function, "led", configMINIMAL_STACK_SIZE, NULL, 1, &led_toggle_task_handle);

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while (true)
    {
        /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
    }
}

/**
 *@}
 **/
