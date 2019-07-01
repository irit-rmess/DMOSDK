# DMOSDK - DecaWave Module Open Software Development Kit
# Copyright (C) 2018-2019 IRIT-RMESS
#
# This file is part of DMOSDK.
#
# DMOSDK is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DMOSDK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DMOSDK.  If not, see <http://www.gnu.org/licenses/>.


OPENOCD_CFG = $(BOARD_PATH)/openocd.cfg

################################################################################
#  Flash targets section
################################################################################

flash:
	$(OPENOCD) -d0 -s $(OPENOCD_DIR) -f $(OPENOCD_CFG) -c "program build/firmware.hex verify reset; shutdown;"

eraseall:
	$(OPENOCD) -s $(OPENOCD_DIR) -f $(OPENOCD_CFG) -c "init; halt; nrf51 mass_erase; shutdown;"

openocd:
	$(OPENOCD) -s $(OPENOCD_DIR) -f $(OPENOCD_CFG)

debug:
	arm-none-eabi-gdb build/firmware.elf -ex "target extended-remote :3333"
