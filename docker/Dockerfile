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

FROM ubuntu:19.04
LABEL Description="Image bundling all the compilation tools for DMOSDK"

WORKDIR /DMOSDK

# Install any needed packages specified in requirements.txt
RUN apt update && \
    apt upgrade -y && \
    apt install -y \
# Development files
      build-essential \
      bzip2 \
      libncurses5 \
      curl \
      python3 && \
    apt clean

RUN curl https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/8-2018q4/gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2 --compressed | tar xj -C /

ENV PATH "/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH"
