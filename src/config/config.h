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

/**
 * @file
 * @brief Utility to manage a JSON config in flash
 *
 * Config format:
 * \0 CONFIG \0 <size (uint16_t)> <json> \0
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#define JSMN_HEADER
#include "jsmn.h"

typedef struct
{
    const char * const buffer;
    uint16_t size;
    jsmntok_t * tokens;
    int num_tokens;
} config_t;

enum config_read_codes
{
    CONFIG_PARSED = 0,
    CONFIG_HEADER_ERROR = 1,
    CONFIG_JSON_PARSE_ERROR = 2
};

extern config_t config;

/**
 * @brief Reads config from flash
 *
 * Parses config as JSON
 * @return ::config_read_codes
 */
int config_read();

/**
 * @brief Writes config to flash
 *
 * @param[in] buffer
 * @param[in] len
 * @return number of bytes written to flash
 */
int config_write(char *buffer, uint16_t len);
#endif
