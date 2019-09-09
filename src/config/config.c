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

#include "nrfx_nvmc.h"

#include "serial.h"
#include "logger.h"

#include "json.h"

#include "config.h"

#define LOG_CONFIG(severity, format, ...) log(LOG_FACILITY_OTHER, severity, "{\"util\":\"config\",\"message\":" format "}" __VA_OPT__(,) __VA_ARGS__)

#define SOS_SIZE 8
#define HEADER_SIZE (SOS_SIZE + sizeof(uint16_t))
#define ALIGNED_HEADER_SIZE (HEADER_SIZE + (HEADER_SIZE % sizeof(uint32_t)))
static char header[HEADER_SIZE] = "\0CONFIG";

extern char __config_section_start[];

static const char * const config_data_start = __config_section_start + ALIGNED_HEADER_SIZE;

static jsmntok_t tokens[64]; /* We expect no more than 64 tokens */
static parser_t parser = PARSER(tokens);

config_t config = {.buffer = config_data_start};

int config_read()
{
    if (strncmp(__config_section_start + 1, header + 1, SOS_SIZE - 1) != 0)
    {
        LOG_CONFIG(LOG_SEVERITY_ERROR, STRING("Header corrupted"));
        return CONFIG_HEADER_ERROR;
    }

    config.size = (uint16_t) *(__config_section_start + SOS_SIZE);

    int res = json_parse(
                  &parser, (char *)config_data_start,
                  config.size,
                  true
              );

    if (!(res > 0))
    {
        LOG_CONFIG(LOG_SEVERITY_ERROR, STRING("JSON parsing failed"));
        config.num_tokens = 0;
        return CONFIG_JSON_PARSE_ERROR;
    }
    config.tokens = tokens;
    config.num_tokens = res;

    return CONFIG_PARSED;
}

int config_write(char *buffer, uint16_t len)
{
    *((uint16_t *) (header + SOS_SIZE)) = len;
    buffer[len] = 0;
    nrfx_nvmc_page_erase((uint32_t)__config_section_start);
    nrfx_nvmc_bytes_write((uint32_t)config_data_start, buffer, len + 1);
    while (!nrfx_nvmc_write_done_check());
    nrfx_nvmc_bytes_write((uint32_t)__config_section_start, header, ALIGNED_HEADER_SIZE);
    while (!nrfx_nvmc_write_done_check());
    return ALIGNED_HEADER_SIZE + len + 1;
}
