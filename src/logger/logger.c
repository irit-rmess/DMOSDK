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

#include "FreeRTOS.h"
#include "task.h"

#include "printf.h"
#include "config.h"
#include "json.h"
#include "rtc.h"
#include "logger.h"

static char severity_desc[][10] = {
    "error",
    "warning",
    "info",
    "debug"
};

static log_severity_t logger_severity = LOG_SEVERITY_ERROR;

void set_logger_severity(log_severity_t severity)
{
    logger_severity = severity;
}

void log(log_severity_t severity, const char * format, ...)
{
    if (severity > logger_severity) return;

    timestamp_t timestamp = rtc_is_inialized() ? rtc_timestamp() : 0;

    pprintf(
            "{\"command\":\"log\",\"payload\":{\"timestamp\":%llu,\"task\":\"%s\",\"severity\":\"%s\",\"message\":",
            timestamp,
            pcTaskGetName(xTaskGetCurrentTaskHandle()),
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
