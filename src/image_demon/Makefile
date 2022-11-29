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
BINS = $(SOURCES:$(SRC_PATH)/%.c=$(BIN_PATH)/%)

LDFLAG += -Wl,-gc-sections

all: 	$(BINS)

$(BINS): $(OBJECTS)
	$(CPLUSPLUS) $(LDFLAG) -o $@ $< $(LIBS)
	$(STRIP) $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ_PATH)/* $(BIN_PATH)/*