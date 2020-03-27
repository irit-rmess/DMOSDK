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

#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#include "json.h"
#include "logger.h"
#include "strntol.h"

#include "heartbeat.h"

static TaskHandle_t heartbeat_task_handle;

static uint32_t period = HEARTBEAT_DEFAULT_PERIOD;
static int led_gpio = HEARTBEAT_DEFAULT_LED_GPIO;

/**
 * @brief Configures the GPIO for the LED
 */
static void configure_led_gpio()
{
    nrf_gpio_cfg_output(led_gpio);
}

/**
 * @brief Resets the GPIO for the LED
 */
static void reset_led_gpio()
{
    nrf_gpio_cfg_default(led_gpio);
}

/**
 * @brief Toggles the LED
 */
static void toggle_led()
{
    nrf_gpio_pin_toggle(led_gpio);
}

/**
 * @brief Task handling a heartbeat
 *
 * @param[in] pvParameter Unused
 */
static void heartbeat_task(void *pvParameter)
{
    while (true)
    {
        if (led_gpio >= 0)
            toggle_led();

        LOG(LOG_SEVERITY_INFO, STRING("Heartbeat"));

        /* Delay a task for a given number of ticks */
        vTaskDelay(MS_TO_TICKS(period));

        /* Tasks must be implemented to never return... */
    }
}

void set_period(uint32_t p)
{
    period = p;
}

void set_led_gpio(int pin)
{
    reset_led_gpio();
    led_gpio = pin;
    configure_led_gpio();
}

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

int heartbeat_init()
{
    configure_led_gpio();

    BaseType_t task_creation_status = xTaskCreate(
            heartbeat_task,
            "heartbeat",
            configMINIMAL_STACK_SIZE,
            NULL,
            1,
            &heartbeat_task_handle
    );

    return task_creation_status == pdPASS;
}
