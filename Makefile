.PHONY: all clean

CONFIG_UCLIBC_BUILD=n
CROSS_COMPILE = toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2
STRIP = $(CROSS_COMPILE)strip

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
endif

SDK_LIB_DIR = sdk/lib/uclibc

INCLUDES = -I$(SDK_INC_DIR)
SDK_INC_DIR = sdk/include
LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a
BUILD_DIR = build/picture_processing
BIN_DIR = bin/picture_processing
SRC_DIR = src/picture_processing
LDFLAG += -Wl,-gc-sections

all: $(BIN_DIR)/shoot_pic_raw

$(BIN_DIR)/shoot_pic_raw: $(BUILD_DIR)/system.o $(BUILD_DIR)/shoot_pic_raw.o $(SRC_DIR)/system.h
	$(CC) -o $@ $^ $(LDFLAG) $(CFLAGS) $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

$(BUILD_DIR)/system.o: $(SRC_DIR)/system.c $(SRC_DIR)/system.h
	$(CC) -c $(SRC_DIR)/system.c $(CFLAGS) -o $(BUILD_DIR)/system.o

$(BUILD_DIR)/shoot_pic_raw.o: $(SRC_DIR)/shoot_pic_raw.c $(SRC_DIR)/system.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(BIN_DIR)/* $(BUILD_DIR)/*