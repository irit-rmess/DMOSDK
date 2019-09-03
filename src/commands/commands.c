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

#include "printf.h"
#include "logger.h"
#include "json.h"

#include "commands.h"

#define LOG_CMD(severity, format, ...) log(LOG_FACILITY_OTHER, severity, "{\"util\":\"cmd\",\"message\":" format "}" __VA_OPT__(,) __VA_ARGS__)

int cmd_help(char *buffer, jsmntok_t *pld_tok);
int cmd_config(char *buffer, jsmntok_t *pld_tok);
int cmd_reboot(char *buffer, jsmntok_t *pld_tok);
int cmd_send(char *buffer, jsmntok_t *pld_tok);

json_cmd_desc_t json_cmd_desc_table[] = {
    {"help", cmd_help, "Gives help on the commands"},
    {"config", cmd_config, "Configure the node"},
    {"reboot", cmd_reboot, "Reboots the node"},
    {"send", cmd_send, "Sends a message"},
    {NULL, NULL, NULL}
};

int json_parse_command(char *buffer, jsmntok_t *tokens, int num_tokens)
{
    jsmntok_t *cmd_tok = NULL;
    jsmntok_t *pld_tok = NULL;
    /* Loop over all keys of the root object */
    for (int i = 1; i < num_tokens; i++)
    {
        if (jsoneq(buffer, &tokens[i], "command") == 0)
        {
            cmd_tok = &tokens[++i];
        }
        else if (jsoneq(buffer, &tokens[i], "payload") == 0)
        {
            pld_tok = &tokens[++i];
        }
        if (cmd_tok != NULL && pld_tok != NULL)
        {
            break;
        }
    }
    if (cmd_tok == NULL)
    {
        LOG_CMD(LOG_SEVERITY_ERROR, STRING("Incorrect JSON format"));
        return JSON_INCORRECT_FORMAT;
    }

    handle_json_command(buffer, cmd_tok, pld_tok);
    return JSON_PARSED;
}

void handle_json_command(char *buffer, jsmntok_t *cmd_tok, jsmntok_t *pld_tok)
{
    int i = 0;
    while (json_cmd_desc_table[i].cmd != NULL)
    {
        if (jsoneq(buffer, cmd_tok, json_cmd_desc_table[i].name) == 0)
        {
            json_cmd_desc_table[i].cmd(buffer, pld_tok);
            return;
        }
        i++;
    }
}

/**
 * @brief Gives help on the commands
 *
 * @param[in] buffer
 * @param[in] pld_tok Payload token
 */
int cmd_help(char *buffer, jsmntok_t *pld_tok)
{
    int i = 0;
    if (json_cmd_desc_table[i].cmd == NULL) return 0;
    pprintf("{\"command\":\"help\",\"payload\":{\"commands\": [");
    while (json_cmd_desc_table[i+1].cmd != NULL)
    {
        pprintf("{\"name\":\"%s\",\"description\":\"%s\"},",
                json_cmd_desc_table[i].name, json_cmd_desc_table[i].help);
        i++;
    }
    printf("{\"name\":\"%s\",\"description\":\"%s\"}]}}\r\n",
            json_cmd_desc_table[i].name, json_cmd_desc_table[i].help);
    return 0;
}

int cmd_config(char *buffer, jsmntok_t *pld_tok)
{
    // TODO
    return 0;
}

int cmd_reboot(char *buffer, jsmntok_t *pld_tok)
{
    // TODO
    return 0;
}

int cmd_send(char *buffer, jsmntok_t *pld_tok)
{
    // TODO
    return 0;
}
