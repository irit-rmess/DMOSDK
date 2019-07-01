#!/usr/bin/python3
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

import sys

kconfig_file = "mainmenu \"DMOSDK\"\n"

for filename in sys.argv[2:]:
    with open(filename, 'r') as f:
        for line in f:
            if line[0] is '#':
                continue
            kconfig_file += line

with open(sys.argv[1], 'w') as outfile:
    outfile.write(kconfig_file)
