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

#include "nrfx_uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "semphr.h"

#include "serial.h"

#define TX_PIN 5  // DWM1001 module pin 20
#define RX_PIN 11 // DWM1001 module pin 18

#define SERIAL_SEND_TASK_PRIORITY 2
#define SERIAL_RECEIVE_TASK_PRIORITY 3

#define SEND_STREAM_BUFFER_SIZE 512
#define RECEIVE_STREAM_BUFFER_SIZE 512

#define UART_TX_BUFFER_SIZE 512

#define NOTIFY_UART_RX_DONE 1
#define NOTIFY_UART_ERROR 2

static nrfx_uart_t serial = NRFX_UART_INSTANCE(0);

static StreamBufferHandle_t send_stream_buffer;
static StreamBufferHandle_t receive_stream_buffer;

static SemaphoreHandle_t writing;
static SemaphoreHandle_t reading;

static TaskHandle_t serial_send_task_handle;
static TaskHandle_t serial_receive_task_handle;

/**
 * @brief UART event handler notifying the serial tasks
 *
 * @param[in] p_event nrfx UART event
 * @param[in] p_context Unused
 */
static void uart_event_handler(nrfx_uart_event_t const * p_event, void * p_context)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch(p_event->type)
    {
        case NRFX_UART_EVT_RX_DONE:
            xTaskNotifyFromISR(serial_receive_task_handle,
                    NOTIFY_UART_RX_DONE,
                    eSetValueWithOverwrite,
                    &xHigherPriorityTaskWoken);
            break;
        case NRFX_UART_EVT_TX_DONE:
            xTaskNotifyFromISR(serial_send_task_handle, 0, eNoAction,
                    &xHigherPriorityTaskWoken);
            break;
        case NRFX_UART_EVT_ERROR:
            xTaskNotifyFromISR(serial_receive_task_handle,
                    NOTIFY_UART_ERROR,
                    eSetValueWithOverwrite,
                    &xHigherPriorityTaskWoken);
            break;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Task handling sends on the serial
 *
 * Sends, using the nrfx driver, bytes received from a stream buffer.
 *
 * @param[in] pvParameter Unused
 */
static void serial_send_task(void * pvParameter)
{
    static uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
    while (true)
    {
        size_t bytes_to_transmit = xStreamBufferReceive(send_stream_buffer,
                uart_tx_buffer, UART_TX_BUFFER_SIZE, portMAX_DELAY);
        nrfx_uart_tx(&serial, uart_tx_buffer, bytes_to_transmit);
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

/**
 * @brief Task handling receives on the serial
 *
 * Transfers bytes received by the nrfx drivers to a stream buffer
 *
 * @param[in] pvParameter Unused
 */
static void serial_receive_task(void * pvParameter)
{
    static uint8_t uart_rx_buffer;
    while (true)
    {
        uint32_t notification_value;
        nrfx_uart_rx(&serial, &uart_rx_buffer, 1);
        xTaskNotifyWait(0, 0, &notification_value, portMAX_DELAY);
        if (notification_value == NOTIFY_UART_ERROR) continue;
        size_t bytes = xStreamBufferSend(receive_stream_buffer,
                &uart_rx_buffer, 1, 0);
        if (bytes == 0)
            serial_send("Error: Receive stream buffer full\r\n", 35);
    }
}

int serial_init()
{
    nrfx_uart_config_t config = NRFX_UART_DEFAULT_CONFIG;
    config.pseltxd = TX_PIN;
    config.pselrxd = RX_PIN;
    nrfx_err_t res = nrfx_uart_init(&serial, &config, uart_event_handler);
    if (res != NRFX_SUCCESS)
        return 1;

    writing = xSemaphoreCreateMutex();
    if (writing == NULL)
        return 1;

    reading = xSemaphoreCreateMutex();
    if (reading == NULL)
        return 1;

    send_stream_buffer = xStreamBufferCreate(SEND_STREAM_BUFFER_SIZE, 1);
    if (send_stream_buffer == NULL)
        return 1;

    receive_stream_buffer = xStreamBufferCreate(RECEIVE_STREAM_BUFFER_SIZE, 1);
    if (receive_stream_buffer == NULL)
        return 1;

    BaseType_t task_creation_status = xTaskCreate(
            serial_send_task,
            "serial_send",
            configMINIMAL_STACK_SIZE,
            NULL,
            SERIAL_SEND_TASK_PRIORITY,
            &serial_send_task_handle
    );
    if (task_creation_status != pdPASS)
        return 1;

    task_creation_status = xTaskCreate(
            serial_receive_task,
            "serial_receive",
            configMINIMAL_STACK_SIZE,
            NULL,
            SERIAL_RECEIVE_TASK_PRIORITY,
            &serial_receive_task_handle
    );
    if (task_creation_status != pdPASS)
        return 1;

    return 0;
}

int serial_send(char * data, size_t length)
{
    size_t bytes_sent = 0;
    xSemaphoreTake(writing, portMAX_DELAY);
    bytes_sent = xStreamBufferSend(send_stream_buffer, data, length, portMAX_DELAY);
    xSemaphoreGive(writing);
    return bytes_sent;
}

int serial_receive(char * data, size_t length)
{
    if (length > RECEIVE_STREAM_BUFFER_SIZE)
        length = RECEIVE_STREAM_BUFFER_SIZE;
    size_t bytes_received = 0;
    xSemaphoreTake(reading, portMAX_DELAY);
    bytes_received = xStreamBufferReceive(receive_stream_buffer,
                data, length, portMAX_DELAY);
    xSemaphoreGive(reading);
    return bytes_received;
}
