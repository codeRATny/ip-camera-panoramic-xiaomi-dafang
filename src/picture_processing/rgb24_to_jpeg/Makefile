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
LIBS = $(SDK_LIB_DIR)/libjpeg.a
BUILD_DIR = ../../../build/picture_processing
BIN_DIR = ../../../bin/picture_processing
SRC_DIR = .
LDFLAG += -Wl,-gc-sections

all: $(BIN_DIR)/sample_jpeg_encode

$(BIN_DIR)/sample_jpeg_encode: $(BUILD_DIR)/rgb24_jpg.o $(BUILD_DIR)/sample_jpeg_encode.o $(SRC_DIR)/rgb24_jpg.h
	$(CC) -o $@ $^ $(LDFLAG) $(CFLAGS) $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

$(BUILD_DIR)/rgb24_jpg.o: $(SRC_DIR)/rgb24_jpg.c $(SRC_DIR)/rgb24_jpg.h
	$(CC) -std=c99 -c $(SRC_DIR)/rgb24_jpg.c $(CFLAGS) -o $(BUILD_DIR)/rgb24_jpg.o

$(BUILD_DIR)/sample_jpeg_encode.o: $(SRC_DIR)/sample_jpeg_encode.c $(SRC_DIR)/rgb24_jpg.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(BIN_DIR)/* $(BUILD_DIR)/*