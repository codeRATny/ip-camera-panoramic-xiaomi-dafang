.PHONY: all clean

CROSS_COMPILE = toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar rc
STRIP = $(CROSS_COMPILE)strip

SRC_PATH = src
BUILD_PATH = build
BIN_PATH = bin

MODULES_PATH = src/picture_processing
MOTOR_PATH = src/motor

SDK_LIB_PATH = sdk/lib/uclibc
SDK_INC_PATH = sdk/include

LDFLAG = -Wl,-gc-sections
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

all: $(BUILD_PATH)/libmotor_daemon_api.a $(BIN_PATH)/image_demon_launcher $(BIN_PATH)/test_client $(BIN_PATH)/image_demon $(BIN_PATH)/api_motor_mq $(BIN_PATH)/daemon_motor

$(BIN_PATH)/image_demon: $(BUILD_PATH)/image_demon.o $(BUILD_PATH)/nv_12_img.o $(SDK_LIB_PATH)/libimp.a $(SDK_LIB_PATH)/libalog.a $(BUILD_PATH)/nv12_rgb24.o $(BUILD_PATH)/rgb24_jpg.o $(SDK_LIB_PATH)/libjpeg.a
	$(CC) $(LDFLAG) -g -rdynamic -I $(MODULES_PATH) -I $(MOTOR_PATH) -o $(BIN_PATH)/image_demon $(BUILD_PATH)/image_demon.o  $(BUILD_PATH)/nv_12_img.o $(SDK_LIB_PATH)/libimp.a $(SDK_LIB_PATH)/libalog.a $(BUILD_PATH)/nv12_rgb24.o $(BUILD_PATH)/rgb24_jpg.o $(SDK_LIB_PATH)/libjpeg.a $(LIBS) -L$(BUILD_PATH)/ -lmotor_daemon_api -lrt -lm -lpthread -static

$(BIN_PATH)/test_client: $(BUILD_PATH)/test_client.o
	$(CC) $(LDFLAG) -o $(BIN_PATH)/test_client $(BUILD_PATH)/test_client.o $(LIBS) -lrt -static

$(BIN_PATH)/image_demon_launcher: $(BUILD_PATH)/image_demon_launcher.o
	$(CC) $(LDFLAG) -o $(BIN_PATH)/image_demon_launcher $(BUILD_PATH)/image_demon_launcher.o $(LIBS) -lrt -static

$(BIN_PATH)/api_motor_mq: $(BUILD_PATH)/libmotor_daemon_api.a $(BUILD_PATH)/api_motor_mq.o
	$(CC) $(LDFLAG) -o $(BIN_PATH)/api_motor_mq $(BUILD_PATH)/api_motor_mq.o -lm -lrt -L$(BUILD_PATH)/ -lmotor_daemon_api -muclibc

$(BIN_PATH)/daemon_motor: $(BUILD_PATH)/daemon_motor.o
	$(CC) $(LDFLAG) -o $(BIN_PATH)/daemon_motor $(BUILD_PATH)/daemon_motor.o -lrt -lm -static

$(BUILD_PATH)/nv12_rgb24.o: $(MODULES_PATH)/nv12_to_rgb24/nv12_rgb24.c $(MODULES_PATH)/nv12_to_rgb24/nv12_rgb24.h
	$(CC) -std=c99 -c $(MODULES_PATH)/nv12_to_rgb24/nv12_rgb24.c $(CFLAGS) -o $(BUILD_PATH)/nv12_rgb24.o

$(BUILD_PATH)/rgb24_jpg.o: $(MODULES_PATH)/rgb24_to_jpeg/rgb24_jpg.c $(MODULES_PATH)/rgb24_to_jpeg/rgb24_jpg.h
	$(CC) -c $(MODULES_PATH)/rgb24_to_jpeg/rgb24_jpg.c -I$(SDK_INC_PATH) $(CFLAGS) -o $(BUILD_PATH)/rgb24_jpg.o

$(BUILD_PATH)/nv_12_img.o: $(MODULES_PATH)/snap_nv12/nv_12_img.c $(MODULES_PATH)/snap_nv12/nv_12_img.h
	$(CC) -g -rdynamic -c $(MODULES_PATH)/snap_nv12/nv_12_img.c -I$(SDK_INC_PATH) $(CFLAGS) -o $(BUILD_PATH)/nv_12_img.o

# $(BUILD_PATH)/message_queue.o: $(SRC_PATH)/image_demon/message_queue.c
# 	$(CC) -c $(SRC_PATH)/image_demon/message_queue.c $(CFLAGS) -o $(BUILD_PATH)/message_queue.o

$(BUILD_PATH)/image_demon_launcher.o: $(SRC_PATH)/image_demon/image_demon_launcher.c
	$(CC) -c $(SRC_PATH)/image_demon/image_demon_launcher.c $(CFLAGS) -o $(BUILD_PATH)/image_demon_launcher.o

$(BUILD_PATH)/test_client.o: $(SRC_PATH)/image_demon/test_client.c
	$(CC) -c $(SRC_PATH)/image_demon/test_client.c $(CFLAGS) -o $(BUILD_PATH)/test_client.o

$(BUILD_PATH)/image_demon.o: $(SRC_PATH)/image_demon/image_demon.c
	$(CC) -g -rdynamic -std=c99 -c $(SRC_PATH)/image_demon/image_demon.c -I $(MODULES_PATH) -I $(MOTOR_PATH) -I $(SDK_INC_PATH) $(CFLAGS) -o $(BUILD_PATH)/image_demon.o -L$(BUILD_PATH)/ -lmotor_daemon_api

$(BUILD_PATH)/libcontrol_motor.o: $(SRC_PATH)/motor/libcontrol_motor.c
	$(CC) $(LDFLAG) -c -o $(BUILD_PATH)/libcontrol_motor.o $(SRC_PATH)/motor/libcontrol_motor.c

$(BUILD_PATH)/libmotor_daemon_api.a: $(BUILD_PATH)/libcontrol_motor.o
	$(AR) $(BUILD_PATH)/libmotor_daemon_api.a $(BUILD_PATH)/libcontrol_motor.o

$(BUILD_PATH)/api_motor_mq.o: $(SRC_PATH)/motor/api_motor_mq.c
	$(CC) $(LDFLAG) -c -o $(BUILD_PATH)/api_motor_mq.o $(SRC_PATH)/motor/api_motor_mq.c -muclibc -lm -lrt -L$(BUILD_PATH)/ -lmotor_daemon_api

$(BUILD_PATH)/daemon_motor.o: $(SRC_PATH)/motor/daemon_motor.c
	$(CC) $(LDFLAG) -c -o $(BUILD_PATH)/daemon_motor.o $(SRC_PATH)/motor/daemon_motor.c -lrt -lm

clean:
	rm -rf $(BUILD_PATH)/* $(BIN_PATH)/*