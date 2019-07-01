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

#include "serial.h"

/**
 * @brief libc write redefinition
 *
 * Writes bytes on the serial
 * @param file - unused
 * @param data - bytes to write
 * @param length
 * @return number of bytes sent to the serial
 */
int _write(int file, const char * data, int length)
{
    return serial_send((char *) data, length);
}

/**
 * @brief libc read redefinition
 *
 * Reads bytes from the serial
 * @param file - unused
 * @param data - buffer for the received bytes
 * @param length - maximum bytes to receive
 * @return number of bytes received
 */
int _read(int file, char * data, int length)
{
    return serial_receive(data, length);
}
