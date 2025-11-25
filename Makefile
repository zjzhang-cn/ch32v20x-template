##### Project #####

PROJECT			?= app
# The path for generated files
BUILD_DIR		= Build

##### Options #####

# Build with FreeRTOS, y:yes, n:no
USE_FREERTOS	?= n

##### Toolchains #######

GCC_TOOCHAIN	?= /opt/toolchain/riscv-wch-embed/bin/

GCC_PREFIX		?= riscv-wch-elf-

OPENOCD_PATH	?= /opt/openocd/bin/

##### Paths ############
# OpenOCD config file
OPENOCD_CFG 	= wch-riscv.cfg

# Library flags, choose one of the following:
# CH32V203: 	CH32V20x_D6
# CH32V203RB: 	CH32V20x_D8
# CH32V208: 	CH32V20x_D8W
LIB_FLAGS		= CH32V20x_D6
# Linker script
LDSCRIPT		:= ch32v20x/EVT/EXAM/SRC/Ld/Link.ld

##### Source Folders #######

# Include paths
INCLUDES	:= ch32v20x/EVT/EXAM/SRC/Core \
		ch32v20x/EVT/EXAM/SRC/Debug \
		ch32v20x/EVT/EXAM/SRC/Peripheral/inc

# C source folders
CDIRS	:= ch32v20x/EVT/EXAM/SRC/Core \
		ch32v20x/EVT/EXAM/SRC/Debug \
		ch32v20x/EVT/EXAM/SRC/Peripheral/src
# C source files (if there are any single ones)
CFILES := 

# ASM source folders
ADIRS	:=
# ASM single files (if there are any single ones)
AFILES	:= ch32v20x/EVT/EXAM/SRC/Startup/startup_ch32v20x_D6.S

LIBS 	:= 

##### Optional Libraries ############

ifeq ($(USE_FREERTOS),y)
##### FreeRTOS ######
ADIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V

CDIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V

CFILES		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/MemMang/heap_4.c

INCLUDES	+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/include \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V \
			ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions

LDSCRIPT	:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/Ld/Link.ld

AFILES		:= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/Startup/startup_ch32v20x_D6.S		
endif

##### User Code Folder ############
ifeq ($(USE_FREERTOS),y)
INCLUDES	+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User
CDIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User 
ADIRS		+= ch32v20x/EVT/EXAM/FreeRTOS/FreeRTOS_Core/User
AFILES 		+= 
else
INCLUDES	+= ch32v20x/EVT/EXAM/RTC/RTC_Calendar/User
CDIRS		+= ch32v20x/EVT/EXAM/RTC/RTC_Calendar/User
ADIRS		+= ch32v20x/EVT/EXAM/RTC/RTC_Calendar/User
AFILES 		+= 
endif

include ./rules.mk
