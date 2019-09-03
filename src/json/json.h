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

#ifndef JSON_H
#define JSON_H

#include <stdbool.h>

#define JSMN_HEADER
#include "jsmn.h"

enum json_parse_codes
{
    JSON_INCOMPLETE = 0,
    JSON_PARSED = 1,
    JSON_PARSER_ERROR = -1,
    JSON_INCORRECT_FORMAT = -2
};

/**
 * @brief Compares a token with a string
 *
 * @param[in] json Buffer
 * @param[in] tok  Token
 * @param[in] s    String to compare
 * @return 0 if the token matches, -1 if not.
 */
int jsoneq(const char *json, jsmntok_t *tok, const char *s);

/**
 * @brief Parses JSON
 *
 * @param[in] buffer
 * @param[in] len
 * @param[in] reset True resets the parser
 * @return ::json_parse_codes
 */
int json_parse(char *buffer, int len, bool reset);

#endif
