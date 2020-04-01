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

#include "nrfx_clock.h"

#include "clock.h"

/**
 * @brief Clock event handler
 *
 * Unused but necessary for nrfx_clock.
 * @param[in] type Clock event type
 */
static void clock_event_handler(nrfx_clock_evt_type_t type) {}

void clock_lfclk_start()
{
    if (nrfx_clock_lfclk_is_running()) return;
    nrfx_clock_lfclk_start();
    while (!nrfx_clock_lfclk_is_running());
}

int clock_init()
{
    return nrfx_clock_init(clock_event_handler) == NRFX_SUCCESS ? 0 : 1;
}
