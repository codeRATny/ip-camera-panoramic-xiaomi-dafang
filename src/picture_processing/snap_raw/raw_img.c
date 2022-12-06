#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_isp.h>
#include <imp/imp_osd.h>

#include "raw_img.h"

#define CHECK_RET(ret, message) \
    if (ret != 0)               \
    {                           \
        printf(message);        \
        return -1;              \
    }

struct chn_conf chn[FS_CHN_NUM] = {
    {
        .index = CH0_INDEX,
        .enable = CHN0_EN,
        .fs_chn_attr = {
            .pixFmt = PIX_FMT_NV12,
            .outFrmRateNum = SENSOR_FRAME_RATE_NUM,
            .outFrmRateDen = SENSOR_FRAME_RATE_DEN,
            .nrVBs = 3,
            .type = FS_PHY_CHANNEL,

            .crop.enable = CROP_EN,
            .crop.top = 0,
            .crop.left = 0,
            .crop.width = SENSOR_WIDTH,
            .crop.height = SENSOR_HEIGHT,

            .scaler.enable = 0,

            .picWidth = SENSOR_WIDTH,
            .picHeight = SENSOR_HEIGHT,
        },
        .framesource_chn = {DEV_ID_FS, 0, 0},
        .imp_encoder = {DEV_ID_ENC, 0, 0},
    },
    {
        .index = CH1_INDEX,
        .enable = CHN1_EN,
        .fs_chn_attr = {
            .pixFmt = PIX_FMT_NV12,
            .outFrmRateNum = SENSOR_FRAME_RATE_NUM,
            .outFrmRateDen = SENSOR_FRAME_RATE_DEN,
            .nrVBs = 3,
            .type = FS_PHY_CHANNEL,

            .crop.enable = CROP_EN,
            .crop.top = 0,
            .crop.left = 0,
            .crop.width = SENSOR_WIDTH,
            .crop.height = SENSOR_HEIGHT,

            .scaler.enable = 1,
            .scaler.outwidth = SENSOR_WIDTH_SECOND,
            .scaler.outheight = SENSOR_HEIGHT_SECOND,

            .picWidth = SENSOR_WIDTH_SECOND,
            .picHeight = SENSOR_HEIGHT_SECOND,
        },
        .framesource_chn = {DEV_ID_FS, 1, 0},
        .imp_encoder = {DEV_ID_ENC, 1, 0},
    },
};

IMPSensorInfo sensor_info;
int img_system_init()
{
    int ret;
    memset(&sensor_info, 0, sizeof(IMPSensorInfo));
    memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
    sensor_info.cbus_type = SENSOR_CUBS_TYPE;
    memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
    sensor_info.i2c.addr = SENSOR_I2C_ADDR;

    ret = IMP_ISP_Open();
    CHECK_RET(ret, "IMP_ISP_Open() failed\n");

    ret = IMP_ISP_AddSensor(&sensor_info);
    CHECK_RET(ret, "IMP_ISP_AddSensor() failed\n");

    ret = IMP_ISP_EnableSensor();
    CHECK_RET(ret, "IMP_ISP_EnableSensor() failed\n");

    ret = IMP_System_Init();
    CHECK_RET(ret, "IMP_System_Init() failed\\n");

    return 0;
}

int img_system_exit()
{
    int ret;

    ret = IMP_System_Exit();
    CHECK_RET(ret, "IMP_System_Exit() failed\n");

    ret = IMP_ISP_DisableSensor();
    CHECK_RET(ret, "IMP_ISP_DisableSensor() failed\n");

    ret = IMP_ISP_DelSensor(&sensor_info);
    CHECK_RET(ret, "IMP_ISP_DelSensor() failed\n");

    ret = IMP_ISP_DisableTuning();
    CHECK_RET(ret, "IMP_ISP_DisableTuning() failed\n");

    ret = IMP_ISP_Close();
    CHECK_RET(ret, "IMP_ISP_Close() failed\n");

    return 0;
}

int snap_raw_picture_file(const char *filename)
{
    int ret;

    IMPFrameInfo *frame_bak;
    IMPFSChnAttr fs_chn_attr[2];
    FILE *fp;

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("ERR: open file\n");
        return -1;
    }

    ret = img_system_init();

    CHECK_RET(ret, "img_system_init() failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_CreateChn(chn[0].index, &chn[0].fs_chn_attr);
        CHECK_RET(ret, "IMP_FrameSource_CreateChn() error !\n");

        ret = IMP_FrameSource_SetChnAttr(chn[0].index, &chn[0].fs_chn_attr);
        CHECK_RET(ret, "IMP_FrameSource_SetChnAttr() error !\n");
    }

    ret = IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]);
    CHECK_RET(ret, "IMP_FrameSource_GetChnAttr failed\n");

    fs_chn_attr[0].pixFmt = PIX_FMT_RAW;
    ret = IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]);
    CHECK_RET(ret, "IMP_FrameSource_SetChnAttr failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_EnableChn(chn[0].index);
        CHECK_RET(ret, "IMP_FrameSource_EnableChn() error:\n");
    }

    ret = IMP_FrameSource_SetFrameDepth(0, 1);
    CHECK_RET(ret, "IMP_FrameSource_SetFrameDepth failed\n");

    ret = IMP_FrameSource_GetFrame(0, &frame_bak);
    CHECK_RET(ret, "IMP_FrameSource_GetFrame failed\n");

    fwrite((void *)frame_bak->virAddr, frame_bak->size, 1, fp);
    fclose(fp);

    ret = IMP_FrameSource_ReleaseFrame(0, frame_bak);
    CHECK_RET(ret, "IMP_FrameSource_ReleaseFrame failed\n");

    ret = IMP_FrameSource_SetFrameDepth(0, 0);
    CHECK_RET(ret, "IMP_FrameSource_SetFrameDepth failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_DisableChn(chn[0].index);
        CHECK_RET(ret, "IMP_FrameSource_DisableChn() error\n");
    }

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_DestroyChn(0);
        CHECK_RET(ret, "IMP_FrameSource_DestroyChn()\n");
    }

    ret = img_system_exit();
    CHECK_RET(ret, "img_system_exit() failed\n");

    return 0;
}

// err: dsys_func_share_mem_register,78 fid register already name = fs_info
// err: dsys_func_user_mem_register,102 fid register already name = misc_save_pic
// err: dsys_func_share_mem_register,78 fid register already name = misc_system_info
// err: dsys_func_share_mem_register,78 fid register already name = enc_info
// err: dsys_func_share_mem_register,78 fid register already name = enc_rc_s
int snap_raw_picture_memory(uint8_t **img)
{
    int ret;

    IMPFrameInfo *frame_bak;
    IMPFSChnAttr fs_chn_attr[2];

    ret = img_system_init();

    CHECK_RET(ret, "img_system_init() failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_CreateChn(chn[0].index, &chn[0].fs_chn_attr);
        CHECK_RET(ret, "IMP_FrameSource_CreateChn() error !\n");

        ret = IMP_FrameSource_SetChnAttr(chn[0].index, &chn[0].fs_chn_attr);
        CHECK_RET(ret, "IMP_FrameSource_SetChnAttr() error !\n");
    }

    ret = IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]);
    CHECK_RET(ret, "IMP_FrameSource_GetChnAttr failed\n");

    fs_chn_attr[0].pixFmt = PIX_FMT_RAW;
    ret = IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]);
    CHECK_RET(ret, "IMP_FrameSource_SetChnAttr failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_EnableChn(chn[0].index);
        CHECK_RET(ret, "IMP_FrameSource_EnableChn() error:\n");
    }

    ret = IMP_FrameSource_SetFrameDepth(0, 1);
    CHECK_RET(ret, "IMP_FrameSource_SetFrameDepth failed\n");
    
    ret = IMP_FrameSource_GetFrame(0, &frame_bak);
    CHECK_RET(ret, "IMP_FrameSource_GetFrame failed\n");

    memcpy(*img, (void *)frame_bak->virAddr, frame_bak->size);

    ret = IMP_FrameSource_ReleaseFrame(0, frame_bak);
    CHECK_RET(ret, "IMP_FrameSource_ReleaseFrame failed\n");

    ret = IMP_FrameSource_SetFrameDepth(0, 0);
    CHECK_RET(ret, "IMP_FrameSource_SetFrameDepth failed\n");

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_DisableChn(chn[0].index);
        CHECK_RET(ret, "IMP_FrameSource_DisableChn() error\n");
    }

    if (chn[0].enable)
    {
        ret = IMP_FrameSource_DestroyChn(0);
        CHECK_RET(ret, "IMP_FrameSource_DestroyChn()\n");
    }

    ret = img_system_exit();
    CHECK_RET(ret, "img_system_exit() failed\n");

    return 0;
}