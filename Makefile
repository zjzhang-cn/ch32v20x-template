##### Project #####

PROJECT			?= app
# The path for generated files
BUILD_DIR		= Build

##### Options #####

# Build with FreeRTOS, y:yes, n:no
USE_FREERTOS	?= y


##### Toolchains #######

GCC_TOOCHAIN	?= /opt/toolchain/riscv-wch-embed/bin/

GCC_PREFIX		?= riscv-wch-elf-

OPENOCD_PATH	?= /opt/openocd/bin/

##### Paths ############

LDSCRIPT		= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/Ld/Link.ld
OPENOCD_CFG 	= wch-riscv.cfg
# CH32V203: 	CH32V20x_D6
# CH32V203RB: 	CH32V20x_D8
# CH32V208: 	CH32V20x_D8W
LIB_FLAGS		= CH32V20x_D6

# C source folders
CDIRS	:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User \
		ch32v20x/EVT/EXAM/SRC/Core \
		ch32v20x/EVT/EXAM/SRC/Debug \
		ch32v20x/EVT/EXAM/SRC/Peripheral/src
# C source files (if there are any single ones)
CFILES := 

# ASM source folders
ADIRS	:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User
# ASM single files (if there are any single ones)
AFILES	:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/Startup/startup_ch32v20x_D6.S

LIBS 	:= 

# Include paths
INCLUDES	:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User \
		ch32v20x/EVT/EXAM/SRC/Core \
		ch32v20x/EVT/EXAM/SRC/Debug \
		ch32v20x/EVT/EXAM/SRC/NetLib \
		ch32v20x/EVT/EXAM/SRC/Peripheral/inc
##### Optional Libraries ############

ifeq ($(USE_FREERTOS),y)
ADIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V

CDIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V

CFILES		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/MemMang/heap_4.c

INCLUDES	+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/include \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions
endif


include ./rules.mk
