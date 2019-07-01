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
 * @brief Hello World FreeRTOS example application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to print "Hello World".
 *
 */

#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "serial.h"

#define TASK_DELAY 1000

TaskHandle_t  hello_world_task_handle;

/**@brief Hello world task entry function.
 *
 * Indefinitely prints "Hello World" with a delay.
 * @param[in] pvParameter Pointer that will be used as the parameter for the task.
 */
static void hello_world_task_function (void * pvParameter)
{
    while (true)
    {
        printf("Hello World\r\n");

        /* Delay a task for a given number of ticks */
        vTaskDelay(TASK_DELAY);

        /* Tasks must be implemented to never return... */
    }
}

int main(void)
{
    serial_init();

    xTaskCreate(
            hello_world_task_function,
            "hello",
            configMINIMAL_STACK_SIZE,
            NULL,
            1,
            &hello_world_task_handle
    );

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
