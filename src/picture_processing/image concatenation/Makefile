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

all: concat

concat: img_concat.o concat.o img_concat.h
	$(CC) -o $@ $^ $(LDFLAG) $(CFLAGS) $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

img_concat.o: img_concat.c img_concat.h
	$(CC) -std=c99 -c img_concat.c $(CFLAGS) -o img_concat.o

concat.o: concat.c img_concat.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(BIN_DIR)/* $(BUILD_DIR)/*