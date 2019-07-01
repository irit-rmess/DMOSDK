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

from kconfiglib import Kconfig, _BOOL_TRISTATE, STRING, HEX, escape
import sys
import re

def write_c_header(kconfig, filename,
                       header="/* Generated by Kconfiglib (https://github.com/ulfalizer/Kconfiglib) */\n"):
        r"""
        Writes out symbol values as a C header file.
        Based on Kconfig's write_autoconf function.

        The ordering of the #defines matches the one generated by
        write_config(). The order in the C implementation depends on the hash
        table implementation as of writing, and so won't match.

        filename:
          Self-explanatory.

        header (default: "/* Generated by Kconfiglib (https://github.com/ulfalizer/Kconfiglib) */\n"):
          Text that will be inserted verbatim at the beginning of the file. You
          would usually want it enclosed in '/* */' to make it a C comment,
          and include a final terminating newline.
        """
        with kconfig._open(filename, "w") as f:
            f.write(header)

            for sym in kconfig.unique_defined_syms:
                # Note: _write_to_conf is determined when the value is
                # calculated. This is a hidden function call due to
                # property magic.
                val = sym.str_value
                if sym._write_to_conf:
                    if re.match('PREDEFINED_', sym.name):
                        continue
                    if sym.orig_type in _BOOL_TRISTATE:
                        f.write("#define {}{}{} {}\n"
                                .format(kconfig.config_prefix, sym.name,
                                        "_MODULE" if val == "m" else "",
                                        "1" if val == "y" else "0"))

                    elif sym.orig_type is STRING:
                        f.write('#define {}{} "{}"\n'
                                .format(kconfig.config_prefix, sym.name,
                                        escape(val)))

                    else:  # sym.orig_type in _INT_HEX:
                        if sym.orig_type is HEX and \
                           not val.startswith(("0x", "0X")):
                            val = "0x" + val

                        f.write("#define {}{} {}\n"
                                .format(kconfig.config_prefix, sym.name, val))


config = Kconfig(sys.argv[1])
config.config_prefix = ""

try:
    config.load_config('.config')
except:
    print("Error: failed to open .config file, run 'make menuconfig' first.")
    sys.exit(1)

write_c_header(config, "build_config.h", header="")
