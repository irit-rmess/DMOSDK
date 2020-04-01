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

#include "config.h"
#include "json.h"

#include "heartbeat.h"

void heartbeat_load_saved_config()
{
    for (int i = 0; i < config.num_tokens; i++)
    {
        if (jsoneq(config.buffer, config.tokens + i, "heartbeat") == 0)
        {
            ++i;
            jsmntok_t *t_log = config.tokens + i;
            ++i;
            for (int child = 0; child < t_log->size; child++)
            {
                jsmntok_t *t_child = config.tokens + i;
                if (jsoneq(config.buffer, config.tokens + i, "period") == 0)
                {
                    ++i;
                    jsmntok_t *t_per = config.tokens + i;
                    set_period(strntol(config.buffer + t_per->start, t_per->end - t_per->start, NULL, 10));
                }
                if (jsoneq(config.buffer, config.tokens + i, "led_gpio") == 0)
                {
                    ++i;
                    jsmntok_t *t_led_gpio = config.tokens + i;
                    set_led_gpio(strntol(config.buffer + t_led_gpio->start, t_led_gpio->end - t_led_gpio->start, NULL, 10));
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
