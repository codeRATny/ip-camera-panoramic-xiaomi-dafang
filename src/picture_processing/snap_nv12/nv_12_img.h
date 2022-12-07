#pragma once

#include <imp/imp_common.h>
#include <imp/imp_osd.h>
#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <imp/imp_encoder.h>
#include <unistd.h>

#define SENSOR_NAME "jxf22"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x40
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CROP_EN 1

#define CH0_INDEX 0

struct chn_conf
{
    unsigned int index;
    unsigned int enable;
    IMPEncoderProfile payloadType;
    IMPFSChnAttr fs_chn_attr;
    IMPCell framesource_chn;
    IMPCell imp_encoder;
};

int sample_system_init();
int sample_system_exit();
int snap_yuv_nv21_init();
int snap_yuv_nv21(uint8_t **img);
int snap_yuv_nv21_exit();
