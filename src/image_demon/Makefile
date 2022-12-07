CROSS_COMPILE = ../../toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
MODULES_INC_DIR = ../picture_processing
INCLUDES = -I$(MODULES_INC_DIR)
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2
# CFLAGS += -muclibc
# LDFLAG += -muclibc
SDK_INC_DIR = ../../sdk/include
SDK_INLUDE = -I$(SDK_INC_DIR)

WORK_DIR = ../..
BIN_PATH = ${WORK_DIR}/bin/image_demon
OBJ_PATH = ${WORK_DIR}/build/image_demon
SRC_PATH = ${WORK_DIR}/src/image_demon
MODULES_PATH = ../picture_processing

SDK_LIB_DIR = ../../sdk/lib/uclibc
LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a
LIBS2 = $(SDK_LIB_DIR)/libjpeg.a

SOURCES = $(shell find $(SRC_PATH) -name '*.c')
OBJECTS = $(SOURCES:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)
BINS = $(BIN_PATH)/image_demon $(BIN_PATH)/image_demon_launcher $(BIN_PATH)/test_client

LDFLAG += -Wl,-gc-sections

all: 	$(BINS)

$(BIN_PATH)/image_demon: $(OBJ_PATH)/image_demon.o $(OBJ_PATH)/message_queue.o $(OBJ_PATH)/nv_12_img.o $(OBJ_PATH)/nv12_rgb24.o $(OBJ_PATH)/rgb24_jpg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) $(LIBS2) -I$(MODULES_PATH) -lpthread -lm -lrt -static
	$(STRIP) $@

$(BIN_PATH)/image_demon_launcher: $(OBJ_PATH)/image_demon_launcher.o
	$(CC) $(LDFLAG) -o $@ $< $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

$(BIN_PATH)/test_client: $(OBJ_PATH)/test_client.o
	$(CC) $(LDFLAG) -o $@ $< $(LIBS) -lpthread -lm -lrt -static
	$(STRIP) $@

$(OBJ_PATH)/nv_12_img.o: $(MODULES_PATH)/snap_nv12/nv_12_img.c $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a
	$(CC) $(LDFLAG) -o $@ $< $(LIBS) $(SDK_INLUDE) -lpthread -lm -lrt -static
	$(STRIP) $@

$(OBJ_PATH)/nv12_rgb24.o: $(MODULES_PATH)/nv12_to_rgb24/nv12_rgb24.c
	$(CC) -std=c99 $(LDFLAG) $(LIBS2) $(LIBS) -o $@ $< -lpthread -lm -lrt -static
	$(STRIP) $@

$(OBJ_PATH)/rgb24_jpg.o: $(MODULES_PATH)/rgb24_to_jpeg/rgb24_jpg.c
	$(CC) $(CFLAGS) $(LDFLAG) -o $@ $< $(LIBS2) $(SDK_INLUDE) -lpthread -lm -lrt -static
	$(STRIP) $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -std=c99 -c $(CFLAGS) $(SDK_INLUDE) $< -o $@

clean:
	rm -rf $(OBJ_PATH)/* $(BIN_PATH)/*