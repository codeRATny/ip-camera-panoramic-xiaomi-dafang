CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2
CFLAGS += -muclibc
LDFLAG += -muclibc

WORK_DIR = ../..
BIN_PATH = ${WORK_DIR}/bin/image_demon
OBJ_PATH = ${WORK_DIR}/build/image_demon
SRC_PATH = ${WORK_DIR}/src/image_demon

SOURCES = $(shell find $(SRC_PATH) -name '*.c')
OBJECTS = $(SOURCES:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)
BINS = $(BIN_PATH)/image_demon $(BIN_PATH)/image_demon_launcher $(BIN_PATH)/test_client

LDFLAG += -Wl,-gc-sections

all: 	$(BINS)

$(BIN_PATH)/image_demon: $(OBJ_PATH)/image_demon.o $(OBJ_PATH)/message_queue.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lrt
	$(STRIP) $@

$(BIN_PATH)/image_demon_launcher: $(OBJ_PATH)/image_demon_launcher.o
	$(CC) $(LDFLAG) -o $@ $< $(LIBS) -lrt
	$(STRIP) $@

$(BIN_PATH)/test_client: $(OBJ_PATH)/test_client.o
	$(CC) $(LDFLAG) -o $@ $< $(LIBS) -lrt
	$(STRIP) $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ_PATH)/* $(BIN_PATH)/*