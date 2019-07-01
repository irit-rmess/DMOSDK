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

#ifndef DWM1001_H
#define DWM1001_H

#define DW1000_RST    24
#define DW1000_IRQ    19

#define SPIM1_SCK_PIN   16  // DWM1001 SPIM1 sck connected to DW1000
#define SPIM1_MOSI_PIN  20  // DWM1001 SPIM1 mosi connected to DW1000
#define SPIM1_MISO_PIN  18  // DWM1001 SPIM1 miso connected to DW1000
#define SPIM1_SS_PIN    17  // SPI Slave Select GPIO pin number.
#define SPIM1_IRQ_PRIORITY 6

#endif


