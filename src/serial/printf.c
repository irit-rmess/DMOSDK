/* DMOSDK - DecaWave Module Open Software Development Kit
 * Copyright (C) 2018-2020 IRIT-RMESS
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
#include "semphr.h"
#include "task.h"

#include "serial.h"
#include "printf.h"

#define NANOPRINTF_IMPLEMENTATION

// nanoprintf configuration
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
//#define NANOPRINTF_VISIBILITY_STATIC

#include "nanoprintf.h"

#define BUFFER_SIZE 1000

static SemaphoreHandle_t printing;

static char buffer[BUFFER_SIZE];
static int index = 0;

int vpprintf(char const *format, va_list vlist)
{
    if (printing == NULL && ((printing = xSemaphoreCreateMutex()) == NULL))
        return -1;
    if (xSemaphoreGetMutexHolder(printing) != xTaskGetCurrentTaskHandle())
        xSemaphoreTake(printing, portMAX_DELAY);
    int len = vsnprintf(buffer + index, BUFFER_SIZE - index, format, vlist);
    index += len; 
    return len;
}

int pprintf(const char *format, ...)
{
    int len;
    va_list va;
    va_start(va, format);
    len = vpprintf(format, va);
    va_end(va);
    return len;
}

int printf(const char *format, ...)
{
    va_list va;
    va_start(va, format);
    vpprintf(format, va);
    va_end(va);
    int len = serial_send(buffer, index);
    index = 0;
    xSemaphoreGive(printing);
    return len;
}

int snprintf(char *buffer, size_t bufsz, const char *format, ...)
{
    va_list val;
    int rv;
    va_start(val, format);
    rv = npf_vsnprintf(buffer, bufsz, format, val);
    va_end(val);
    return rv;
}

int vsnprintf(char *buffer, size_t bufsz, char const *format, va_list vlist)
{
    return npf_vsnprintf(buffer, bufsz, format, vlist);
}
