.PHONY: all clean

CONFIG_UCLIBC_BUILD=n
CROSS_COMPILE = ../../../toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2
STRIP = $(CROSS_COMPILE)strip

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
endif

SDK_LIB_DIR = ../../../sdk/lib/uclibc

INCLUDES = -I$(SDK_INC_DIR)
SDK_INC_DIR = ../../../sdk/include
LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a
BUILD_DIR = ../../../build/picture_processing
BIN_DIR = ../../../bin/picture_processing
SRC_DIR = .
LDFLAG += -Wl,-gc-sections

all: sample_converter

sample_converter: nv12_rgb24.o sample_converter.o nv12_rgb24.h
	$(CC) -std=c99 -o $@ $^ $(LDFLAG) $(CFLAGS) $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

nv12_rgb24.o: nv12_rgb24.c nv12_rgb24.h
	$(CC) -std=c99 -c nv12_rgb24.c $(CFLAGS) -o nv12_rgb24.o

sample_converter.o: sample_converter.c nv12_rgb24.h
	$(CC) -std=c99 -c $(CFLAGS) $< -o $@

clean:
	rm $(BIN_DIR)/* $(BUILD_DIR)/*