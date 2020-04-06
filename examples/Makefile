# DMOSDK - DecaWave Module Open Software Development Kit
# Copyright (C) 2018-2020 IRIT-RMESS
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

EXAMPLES = blinky hello_world simple_deca_driver
EXAMPLES_CLEAN = $(addsuffix _clean, $(EXAMPLES))

.PHONY: all
all: $(EXAMPLES)

.PHONY: clean
clean: $(EXAMPLES_CLEAN)

.PHONY: $(EXAMPLES)
$(EXAMPLES):
	$(MAKE) -C $@

.PHONY: $(EXAMPLES_CLEAN)
$(EXAMPLES_CLEAN):
	$(MAKE) -C $(patsubst %_clean,%,$@) clean
