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

#ifndef LOGGER_H
#define LOGGER_H

typedef enum
{
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_DEBUG
} log_severity_t;

typedef enum
{
    LOG_FACILITY_APP,
    LOG_FACILITY_OTHER,
    LOG_FACILITY_COUNT
} log_facility_t;

/**
 * @brief Sets logger severity level
 *
 * @param[in] severity Severity level to set
 */
void set_logger_severity(log_severity_t severity);

/**
 * @brief Enable or disable logs for a facility
 *
 * @param[in] facility
 * @param[in] enable True to enable, false to disable
 */
void enable_logger_facility(log_facility_t facility, bool enable);

/**
 * @brief Logs a message
 *
 * Outputs JSON. Depends on printf. The message is timestamped using the RTC.
 * @param[in] facility
 * @param[in] severity
 * @param[in] format Message to log using printf format
 */
void log(log_facility_t facility, log_severity_t severity, const char * format, ...);

/**
 * @brief Logger initialization
 *
 * Configures the LFCLK and the RTC for timestamping messages.
 */
int logger_init();
#endif
