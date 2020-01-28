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

#include <string.h>

#include "jsmn.h"

#include "commands.h"

#include "json.h"


#if LOGGER == 1
#include "logger.h"
#define LOG_JSON(severity, format, ...) log(LOG_FACILITY_OTHER, severity, "{\"util\":\"json\",\"message\":" format "}" __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_JSON(severity, format, ...)
#endif

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int json_parse(parser_t *parser, char *buffer, int len, bool reset)
{
    if (reset) jsmn_init(&(parser->jsmn));

    int r = jsmn_parse(
                &(parser->jsmn),
                buffer, len,
                parser->tokens, parser->num_tokens
            );
    if (r == JSMN_ERROR_NOMEM)
    {
        LOG_JSON(LOG_SEVERITY_ERROR, STRING("Not enough tokens for jsmn parser"));
        return JSON_PARSER_ERROR;
    }
    if (r == JSMN_ERROR_INVAL)
    {
        LOG_JSON(LOG_SEVERITY_ERROR, STRING("Invalid character in JSON string"));
        return JSON_PARSER_ERROR;
    }
    if (r == 0 || r == JSMN_ERROR_PART)
    {
        return JSON_INCOMPLETE;
    }

    if (parser->tokens[0].type != JSMN_OBJECT)
    {
        LOG_JSON(LOG_SEVERITY_ERROR, STRING("JSON Object expected"));
        return JSON_INCORRECT_FORMAT;
    }

    return r;
}
