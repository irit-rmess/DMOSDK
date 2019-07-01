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

#ifndef SERIAL_H
#define SERIAL_H
#include <unistd.h>

/**
 * @brief Serial initialization
 *
 * Configures the UART for the serial. Then, creates the send and receive task
 * with their associated communication primitives.
 */
int serial_init();

/**
 * @brief Enqueues bytes to send over serial
 *
 * Sends the given bytes to the stream buffer for send task. Blocking.
 * @param[in] data Bytes to send
 * @param[in] length
 * @return Number of bytes sent to the stream buffer
 */
int serial_send(char * data, size_t length);

/**
 * @brief Receives bytes over serial
 *
 * Transfers bytes from the receive stream
 * @param[out] data Buffer for the received bytes
 * @param[in] length Maximum bytes to receive
 * @return Number of bytes received
 */
int serial_receive(char * data, size_t length);

#endif

