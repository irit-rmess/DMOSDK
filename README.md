# DMOSDK - DecaWave Module Open Software Development Kit

## Brief
Framework developed by the RMESS team of IRIT to work with industry standard software libraries on Decawave's UWB modules.

## Included software
*  [CMSIS 5](https://github.com/ARM-software/CMSIS_5): ARM's Cortex M HAL
*  [deca_driver](https://github.com/Decawave/dwm1001-examples/tree/master/deca_driver): Decawave's DW1000 HAL
*  [FreeRTOS](https://www.freertos.org/)
*  [nrfx](https://github.com/NordicSemiconductor/nrfx): nRF's SoC HAL
*  [Kconfiglib](https://github.com/ulfalizer/Kconfiglib): Kconfig python implementation

## Features
*  Software configuration through a `menuconfig`
*  Compilation using GCC toolchain
*  Board based debug configuration

## Supported Hardware
*  [DWM1001-DEV](https://www.decawave.com/product/dwm1001-development-board/): Decawave's DWM1001 development board

## Required software
*  `arm-none-eabi-gcc`
*  `make`
*  `python3`

## Getting started
Have a look under the `examples` directory

## Firmware
To generate a binary for the DWM1001:
* Create a directory for your project
* Write a `Makefile` that define `DWM1001_FRAMEWORK_PATH` and includes `build_tools/common.mk`
* Configure the software libraries with `make menuconfig`
* Write your application's code with at least a `main` function
* Compile with `make`

## Code's documentation
Source code documentation is generated using [Doxygen](http://doxygen.nl):
* Go into `doc`
* Run `./generate_all_html_doc.sh`

## Using a Docker image

To avoid dealing with the tools' installation, a Dockerfile to build an all-in-one image is provided under `docker`

Compilation and debug can be done inside a container:
```bash=
docker run -it -v $(pwd):/dmosdk iritrmess/dmosdk:latest
```

To build the image run:
```bash=
docker build -f docker/Dockerfile .
```

## License
DMOSDK is distributed under the GPLv3 license. Please refer to the `LICENSE` file.
