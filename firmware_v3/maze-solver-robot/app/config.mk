# Compile options
VERBOSE=n
OPT=g
USE_NANO=y
SEMIHOST=n
USE_FPU=y

# Libraries
USE_LPCOPEN=y
USE_SAPI=y
LOAD_INRAM=n

DRIVERS_PATH := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../drivers)

SRC += $(DRIVERS_PATH)/src/hc_sr04.c \
       $(DRIVERS_PATH)/src/l298n.c

INC_DIRS += $(DRIVERS_PATH)/inc

