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

#include "nrfx_clock.h"
#include "nrfx_rtc.h"
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

static nrfx_rtc_t const rtc = NRFX_RTC_INSTANCE(0);

static volatile uint64_t overflows;

static log_severity_t logger_severity = LOG_SEVERITY_ERROR;

void set_logger_severity(log_severity_t severity)
{
    logger_severity = severity;
}

void uint64_to_string(uint64_t value, char buffer[])
{
    uint64_t divider = 10;
    while (value / divider)
        divider *= 10;
    int i = 0;
    while (divider != 1)
    {
        buffer[i] = '0' + (value % divider) / (divider / 10);
        divider /= 10;
        i++;
    }
    buffer[i] = 0;
}

void log(log_facility_t facility, log_severity_t severity, const char * format, ...)
{
    if (severity > logger_severity) return;
    uint64_t timestamp = nrfx_rtc_counter_get(&rtc) + (overflows << 24);
    static char timestamp_string[21];
    uint64_to_string(timestamp, timestamp_string);
    static char message[100];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    printf(
            "{\"command\":\"log\",\"payload\":{\"timestamp\":%s,\"facility\":\"%s\",\"severity\":\"%s\",\"message\":\"%s\"}}\r\n",
            timestamp_string,
            facility_desc[facility],
            severity_desc[severity],
            message
    );
}

static void rtc_event_handler(nrfx_rtc_int_type_t type)
{
    if (type == NRFX_RTC_INT_OVERFLOW)
    {
        overflows++;
    }
}

static void clock_event_handler(nrfx_clock_evt_type_t type)
{

}

int logger_init()
{
    nrfx_clock_init(clock_event_handler);
    nrfx_clock_lfclk_start();
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t res = nrfx_rtc_init(&rtc, &config, rtc_event_handler);
    if (res != NRFX_SUCCESS)
        return 1;
    nrfx_rtc_overflow_enable(&rtc, true);
    nrfx_rtc_enable(&rtc);
    return 0;
}
