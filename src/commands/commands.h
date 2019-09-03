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

#ifndef COMMANDS_H
#define COMMANDS_H

#define JSMN_HEADER
#include "jsmn.h"

typedef int (*json_cmd_t)(char *buffer, jsmntok_t *pld_tok);

typedef struct
{
    const char *name;
    json_cmd_t cmd;
    const char *help;
} json_cmd_desc_t;

/**
 * @brief Parses a JSON command
 *
 * Format:
 * @code
 * {
 *   "command": "<command>",
 *   "payload": <payload> 
 * }
 * @endcode
 * @param[in] buffer
 * @param[in] tokens     Tokens from the parser
 * @param[in] num_tokens Number of tokens used
 * @return ::json_parse_codes
 */
int json_parse_command(char *buffer, jsmntok_t *tokens, int num_tokens);

/**
 * @brief Handles a JSON command
 * 
 * @param[in] buffer
 * @param[in] cmd_tok Command token
 * @param[in] pld_tok Payload token
 */
void handle_json_command(char *buffer, jsmntok_t *cmd_tok, jsmntok_t *pld_tok);

#endif
