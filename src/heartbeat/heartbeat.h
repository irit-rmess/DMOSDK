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

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdint.h>

#define HEARTBEAT_DEFAULT_LED_GPIO 22
#define HEARTBEAT_DEFAULT_PERIOD 1000

/**
 * @brief Heartbeat initialization
 *
 * Creates a task that toggles a led periodically
 */
int heartbeat_init();

/**
 * @brief Loads heartbeat parameters from config in flash
 */
void heartbeat_load_saved_config();

/**
 * @brief Sets heartbeat period
 *
 * @param[in] p Period in ms
 */
void set_period(uint32_t p);

/**
 * @brief Sets heartbeat LED gpio
 *
 * @param[in] gpio GPIO number
 */
void set_led_gpio(int gpio);

#endif
