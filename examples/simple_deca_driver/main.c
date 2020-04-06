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
 * @brief Simple Deca Driver FreeRTOS example application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to send/receive a counter value over UWB.
 *
 */
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "serial.h"

#include "deca_device_api.h"
#include "deca_regs.h"
#include "port_platform.h"

#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#define RECEIVER 0
#define SENDER 1

#define ROLE RECEIVER

typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    uint16_t crc;
} message_t;

TaskHandle_t task;

static dwt_config_t dwt_config = {
    5,                /* Channel number. */
    DWT_PRF_64M,      /* Pulse repetition frequency. */
    DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    10,               /* TX preamble code. Used in TX only. */
    10,               /* RX preamble code. Used in RX only. */
    0,                /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_6M8,       /* Data rate. */
    DWT_PHRMODE_STD,  /* PHY header mode. */
    (129 + 8 - 8)     /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

#if (ROLE == RECEIVER)

typedef enum {
    RECEIVE_OK,
    RECEIVE_ERROR
} receive_status_t;

typedef enum {
    DW1000_RX_OK,
    DW1000_RX_TIMEOUT,
    DW1000_RX_ERROR
} dw1000_rx_status_t;

/**
 * @brief Handles events from the DW1000
 *
 * @param[in] pin Unused
 * @param[in] action Unused
 */
static void dw1000_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    dwt_isr();
}

/**
 * @brief Initializes interrupts of the DW1000
 */
static void dw1000_int_init()
{
    /* Setup DW1000 IRQ pin */
    nrf_gpio_cfg_input(DW1000_IRQ, NRF_GPIO_PIN_NOPULL);

    nrfx_gpiote_init(NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY);
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    nrfx_gpiote_in_init(DW1000_IRQ, &config, dw1000_event_handler);
    nrfx_gpiote_in_event_enable(DW1000_IRQ, true);

    /* Minimal set of interruptions */
    dwt_setinterrupt(
        DWT_INT_RFCG |
        DWT_INT_RPHE |
        DWT_INT_RFCE |
        DWT_INT_RFSL |
        DWT_INT_SFDT,
        2 // Overwrites the whole register
    );
}

/**
 * @brief Notifies the task from a DW1000 RX related interrupt
 *
 * @param[in] status DW1000 RX interrupt to notify
 */
static inline void notify_task(dw1000_rx_status_t status)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(task, status, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Deca Driver RX OK callback
 *
 * @param[in] data Unused
 */
void rx_ok_callback(const dwt_cb_data_t * data)
{
    notify_task(DW1000_RX_OK);
}

/**
 * @brief Deca Driver RX TIMEOUT callback
 *
 * @param[in] data Unused
 */
void rx_timeout_callback(const dwt_cb_data_t * data)
{
    notify_task(DW1000_RX_TIMEOUT);
}

/**
 * @brief Deca Driver RX ERROR callback
 *
 * @param[in] data Unused
 */
void rx_error_callback(const dwt_cb_data_t * data)
{
    notify_task(DW1000_RX_ERROR);
}

/**
 * @brief Waits for DW1000 RX interrupt notification
 *
 * @return DW1000 RX interrupt
 */
static dw1000_rx_status_t wait_for_rx()
{
    uint32_t notification;
    xTaskNotifyWait(0, 0, (uint32_t *) &notification, portMAX_DELAY);
    dw1000_rx_status_t status = notification;
    return status;
}

/**
 * @brief Reads the data received as a message
 *
 * @param[in] message pointer to the structure to store the message
 */
void read_message(message_t *message)
{
    dwt_readrxdata((uint8_t *)message, sizeof(message_t), 0);
}

/**
 * @brief Checks that the data size matches with the message
 *
 * @return true if size matches else false
 */
static bool check_message_size()
{
    return (dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023)
        == sizeof(message_t);
}

/**
 * @brief Handles the reception of a message
 *
 * @param[in] message pointer to the structure to store the message
 */
receive_status_t receive_message(message_t *message)
{
    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    if (wait_for_rx() != DW1000_RX_OK)
    {
        dwt_rxreset();
        return RECEIVE_ERROR;
    }

    if (!check_message_size())
    {
        return RECEIVE_ERROR;
    }

    read_message(message);

    return RECEIVE_OK;
}

/**
 * @brief Task handling the reception of messages
 *
 * @param[in] pvParameter Unused
 */
void receive_task(void *pvParameter)
{
    printf("Receiver\r\n");

    dwt_setcallbacks(NULL, rx_ok_callback, rx_timeout_callback, rx_error_callback);

    message_t message;
    while (1)
    {
        if (receive_message(&message) == RECEIVE_OK)
        {
            printf("Received %lu\r\n", message.counter);
        }
    }
}

/**
 * @brief Creates the receive task
 */
void task_create()
{
    xTaskCreate(receive_task, "receive", configMINIMAL_STACK_SIZE, NULL, 1, &task);
}

#else

#define SEND_PERIOD_MS 1000

/**
 * @brief Initializes interrupts of the DW1000
 *
 * Implementation for genericity. DW1000 interrupts are not needed for only sending messages
 */
static void dw1000_int_init() {}

/**
 * @brief Sends a message
 *
 * @param[in] message pointer to the structure of the message to be sent
 */
void send_message(message_t *message)
{
    dwt_writetxdata(sizeof(message_t), (uint8_t *) message, 0); /* Zero offset in TX buffer. */
    dwt_writetxfctrl(sizeof(message_t), 0, 0); /* Zero offset in TX buffer, no ranging. */
    dwt_starttx(DWT_START_TX_IMMEDIATE);
}

/**
 * @brief Task sending messages
 *
 * Increments the counter sent after each transmission
 * @param[in] pvParameter Unused
 */
void send_task(void *pvParameter)
{
    printf("Sender\r\n");
    message_t message = {0};
    while (1)
    {
        printf("Sending %lu\r\n", message.counter);
        send_message(&message);
        vTaskDelay(MS_TO_TICKS(SEND_PERIOD_MS));
        message.counter++;
    }
}

/**
 * @brief Creates the send task
 */
void task_create()
{
    xTaskCreate(send_task, "send", configMINIMAL_STACK_SIZE, NULL, 1, &task);
}

#endif

/**
 * @brief Initializes the DW1000 radio
 */
void dw1000_init()
{
    reset_DW1000();

    /* Set SPI clock to 2MHz */
    port_set_dw1000_slowrate();

    configASSERT(dwt_initialise(DWT_LOADUCODE) != DWT_ERROR)

    // Set SPI to 8MHz clock
    port_set_dw1000_fastrate();

    dwt_configure(&dwt_config);

    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);

    dw1000_int_init();
}

/**
 * @brief Application's entry point
 */
int main(void)
{
    serial_init();

    dw1000_init();

    task_create();

    vTaskStartScheduler();

    // Execution should never reach this point
    while (1);
}

