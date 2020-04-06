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

#include "clock.h"

#include "nrfx_rtc.h"

#include "rtc.h"

static nrfx_rtc_t const rtc = NRFX_RTC_INSTANCE(0);

static volatile uint64_t overflows;

static bool is_initialized = false;

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

bool rtc_is_inialized()
{
    return is_initialized;
}

timestamp_t rtc_timestamp()
{
    return nrfx_rtc_counter_get(&rtc) + (overflows << 24);
}

int rtc_init()
{
    clock_lfclk_start();
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t res = nrfx_rtc_init(&rtc, &config, rtc_event_handler);
    if (res != NRFX_SUCCESS)
        return 1;
    nrfx_rtc_overflow_enable(&rtc, true);
    nrfx_rtc_enable(&rtc);
    is_initialized = true;
    return 0;
}
