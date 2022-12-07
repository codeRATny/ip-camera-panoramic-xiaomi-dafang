#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_isp.h>
#include <imp/imp_osd.h>

#include "nv_12_img.h"

#define CHECK_RET(ret, message) \
    if (ret != 0)               \
    {                           \
        printf(message);        \
        return -1;              \
    }

struct chn_conf chn = {
    .index = CH0_INDEX,
    .enable = CHN0_EN,
    .payloadType = IMP_ENC_PROFILE_HEVC_MAIN,
    .fs_chn_attr = {
        .pixFmt = PIX_FMT_NV12,
        .nrVBs = 2,
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
    .framesource_chn = {DEV_ID_FS, CH0_INDEX, 0},
    .imp_encoder = {DEV_ID_ENC, CH0_INDEX, 0},
};

IMPSensorInfo sensor_info;
int sample_system_init()
{
    memset(&sensor_info, 0, sizeof(IMPSensorInfo));
    memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
    sensor_info.cbus_type = SENSOR_CUBS_TYPE;
    memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
    sensor_info.i2c.addr = SENSOR_I2C_ADDR;

    CHECK_RET(IMP_ISP_Open(), "failed to open ISP\n");

    CHECK_RET(IMP_ISP_AddSensor(&sensor_info), "failed to AddSensor\n");

    CHECK_RET(IMP_ISP_EnableSensor(), "failed to EnableSensor\n");

    CHECK_RET(IMP_System_Init(), "IMP_System_Init failed\n");

    CHECK_RET(IMP_ISP_EnableTuning(), "IMP_ISP_EnableTuning failed\n");

    IMP_ISP_Tuning_SetContrast(128);
    IMP_ISP_Tuning_SetSharpness(128);
    IMP_ISP_Tuning_SetSaturation(128);
    IMP_ISP_Tuning_SetBrightness(128);

    CHECK_RET(IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY), "failed to set running mode\n");

    return 0;
}

int sample_system_exit()
{
    IMP_System_Exit();

    CHECK_RET(IMP_ISP_DisableSensor(), "failed to DisableSensor\n");

    CHECK_RET(IMP_ISP_DelSensor(&sensor_info), "failed to DelSensor\n");

    CHECK_RET(IMP_ISP_DisableTuning(), "IMP_ISP_DisableTuning failed\n");

    CHECK_RET(IMP_ISP_Close(), "failed to close ISP\n");

    return 0;
}

int snap_yuv_nv21_init()
{
    IMPFSChnAttr fs_chn_attr[2];
    CHECK_RET(sample_system_init(), "IMP_System_Init() failed\n");

    if (chn.enable)
    {
        CHECK_RET(IMP_FrameSource_CreateChn(chn.index, &chn.fs_chn_attr), "IMP_FrameSource_CreateChn() error\n");

        CHECK_RET(IMP_FrameSource_SetChnAttr(chn.index, &chn.fs_chn_attr), "IMP_FrameSource_SetChnAttr() error\n");
    }

    CHECK_RET(IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]), "IMP_FrameSource_GetChnAttr failed\n");

    fs_chn_attr[0].pixFmt = PIX_FMT_NV12;

    CHECK_RET(IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]), "IMP_FrameSource_SetChnAttr failed\n");

    if (chn.enable)
    {
        CHECK_RET(IMP_FrameSource_EnableChn(chn.index), "IMP_FrameSource_EnableChn() error\n");
    }

    CHECK_RET(IMP_FrameSource_SetFrameDepth(0, 1), "IMP_FrameSource_SetFrameDepth failed\n");

    return 0;
}

int snap_yuv_nv21(uint8_t **img)
{
    IMPFrameInfo *frame_bak;

    if (*img == NULL)
    {
        printf("NULL pointer\n");
        return -1;
    }

    // int m = 0;

    // for (m = 1; m <= 51; m++)
    // {
        CHECK_RET(IMP_FrameSource_GetFrame(0, &frame_bak), "IMP_FrameSource_GetFrame failed\n");

        // if (m % 50 == 0)
        // {
            memcpy(*img, (void *)frame_bak->virAddr, frame_bak->size);
        // }
// 
        CHECK_RET(IMP_FrameSource_ReleaseFrame(0, frame_bak), "IMP_FrameSource_ReleaseFrame failed\n");
    // }

    return 0;
}

int snap_yuv_nv21_exit()
{
    CHECK_RET(IMP_FrameSource_SetFrameDepth(0, 0), "IMP_FrameSource_SetFrameDepth failed\n");

    if (chn.enable)
    {
        CHECK_RET(IMP_FrameSource_DisableChn(chn.index), "IMP_FrameSource_DisableChn() error\n");
    }

    if (chn.enable)
    {
        CHECK_RET(IMP_FrameSource_DestroyChn(0), "IMP_FrameSource_DestroyChn() error\n");
    }

    CHECK_RET(sample_system_exit(), "sample_system_exit() failed\n");

    return 0;
}