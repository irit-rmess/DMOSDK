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

#include <stdarg.h>

#include "nrfx_clock.h"
#include "nrfx_rtc.h"
#include "printf.h"
#include "config.h"
#include "json.h"
#include "logger.h"

static char severity_desc[][10] = {
    "error",
    "warning",
    "info",
    "debug"
};

static char facility_desc[][10] = {
    "app",
    "sys",
    "other"
};

static bool facility_enable[LOG_FACILITY_COUNT] = {true, true, true};

static nrfx_rtc_t const rtc = NRFX_RTC_INSTANCE(0);

static volatile uint64_t overflows;

static log_severity_t logger_severity = LOG_SEVERITY_ERROR;

/**
 * @brief RTC event handler
 *
 * Handles overflows.
 * @param[in] type RTC event type
 */
static void rtc_event_handler(nrfx_rtc_int_type_t type)
{
    if (type == NRFX_RTC_INT_OVERFLOW)
    {
        overflows++;
    }
}

/**
 * @brief Clock event handler
 *
 * Unused but necessary for nrfx_clock.
 * @param[in] type Clock event type
 */
static void clock_event_handler(nrfx_clock_evt_type_t type) {}

void set_logger_severity(log_severity_t severity)
{
    logger_severity = severity;
}

void enable_logger_facility(log_facility_t facility, bool enable)
{
    if (facility < LOG_FACILITY_COUNT)
        facility_enable[facility] = enable;
}

void log(log_facility_t facility, log_severity_t severity, const char * format, ...)
{
    if (severity > logger_severity) return;
    if (!facility_enable[facility]) return;

    uint64_t timestamp = nrfx_rtc_counter_get(&rtc) + (overflows << 24);
    pprintf(
            "{\"command\":\"log\",\"payload\":{\"timestamp\":%llu,\"facility\":\"%s\",\"severity\":\"%s\",\"message\":",
            timestamp,
            facility_desc[facility],
            severity_desc[severity]
          );

    va_list va; {}
    va_start(va, format);
    vpprintf(format, va);
    va_end(va);

    printf("}}\r\n");
}

void logger_load_saved_config()
{
    for (int i = 0; i < config.num_tokens; i++)
    {
        if (jsoneq(config.buffer, config.tokens + i, "logger") == 0)
        {
            ++i;
            jsmntok_t *t_log = config.tokens + i;
            ++i;
            for (int child = 0; child < t_log->size; child++)
            {
                jsmntok_t *t_child = config.tokens + i;
                if (jsoneq(config.buffer, config.tokens + i, "severity") == 0)
                {
                    ++i;
                    jsmntok_t *t_sev = config.tokens + i;
                    if (jsoneq(config.buffer, t_sev, "debug") == 0)
                    {
                        set_logger_severity(LOG_SEVERITY_DEBUG);
                    }
                    else if (jsoneq(config.buffer, t_sev, "info") == 0)
                    {
                        set_logger_severity(LOG_SEVERITY_INFO);
                    }
                    else if (jsoneq(config.buffer, t_sev, "warning") == 0)
                    {
                        set_logger_severity(LOG_SEVERITY_WARNING);
                    }
                    else if (jsoneq(config.buffer, t_sev, "error") == 0)
                    {
                        set_logger_severity(LOG_SEVERITY_ERROR);
                    }
                    return;
                }
                else
                {
                    // Ignore unknown attributes, find the next sibling
                    ++i;
                    while(i < config.num_tokens
                        && (t_child + 1)->end > config.tokens[i].start)
                        ++i;
                }
            }
            return;
        }
    }
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
