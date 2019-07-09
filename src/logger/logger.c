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

#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

static char severity_desc[][10] = {
    "error",
    "warning",
    "info",
    "debug"
};

static char facility_desc[][10] = {
    "app",
    "other"
};

void log(log_facility_t facility, log_severity_t severity, const char * format, ...)
{
    static char message[100];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    printf(
            "{\"command\":\"log\",\"payload\":{\"timestamp\":%d,\"facility\":\"%s\",\"severity\":\"%s\",\"message\":\"%s\"}}\r\n",
            1234,
            facility_desc[facility],
            severity_desc[severity],
            message
    );
}
