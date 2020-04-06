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

#ifndef PRINTF_H
#define PRINTF_H
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief Prepares a printf
 *
 * Appends the formatted given string in the buffer used by printf.
 * Useful when generating complex strings. Avoids having to deal with buffers.
 * Preparation is task exclusive. The lock is released and the string is sent
 * to the serial when printf is called.
 * @param[in] format
 * @return Number of bytes written in printf's buffer
 */
int pprintf(const char *format, ...);

/**
 * @brief See pprintf(const char *format, ...)
 */
int vpprintf(char const *format, va_list vlist);

/** 
 * @brief stdio printf's counterpart
 */
int printf(const char *format, ...);
/** 
 * @brief stdio snprintf's counterpart
 */
int snprintf(char *buffer, size_t bufsz, const char *format, ...);
/** 
 * @brief stdio vsnprintf's counterpart
 */
int vsnprintf(char *buffer, size_t bufsz, char const *format, va_list vlist);
#endif
