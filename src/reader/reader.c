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

#include "FreeRTOS.h"
#include "task.h"

#include "serial.h"
#include "logger.h"
#include "json.h"

#include "reader.h"

#define BUFFER_SIZE 1000

#define LOG_READER(severity, format, ...) log(LOG_FACILITY_SYS, severity, "{\"task\":\"reader\",\"message\":" format "}" __VA_OPT__(,) __VA_ARGS__)

#define READER_TASK_PRIORITY 1 

static TaskHandle_t reader_task_handle;

/**
 * @brief Task reading characters received by the serial
 *
 * Parses characters as JSON
 * @param[in] pvParameter Unused
 */
static void reader_task(void *pvParameters)
{
    static char buffer[BUFFER_SIZE];
    int i = 0;
    bool reset_parser = true;
    while (1)
    {
        if (i >= BUFFER_SIZE)
        {
            LOG_READER(LOG_SEVERITY_ERROR, STRING("Commands buffer overflow"));
            i = 0;
            reset_parser = true;
        }
        long len = serial_receive(buffer + i, 1);
        if (!(len > 0))
        {
            LOG_READER(LOG_SEVERITY_ERROR, STRING("Read error"));
            continue;
        }
        i += len;

        int res = json_parse(buffer, i, reset_parser);
        if (res == JSON_INCOMPLETE)
        {
            reset_parser = false;
        }
        else
        {
            i = 0;
            reset_parser = true;
        }
	}
}

int reader_init()
{
    BaseType_t task_creation_status = xTaskCreate(
            reader_task,
            "reader",
            configMINIMAL_STACK_SIZE * 2,
            NULL,
            READER_TASK_PRIORITY,
            &reader_task_handle
    );
    if (task_creation_status != pdPASS)
        return 1;
    return 0;
}
