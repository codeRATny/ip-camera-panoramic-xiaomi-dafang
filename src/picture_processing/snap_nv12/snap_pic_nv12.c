#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <stdlib.h>

#include "nv_12_img.h"

#define IMGSIZE 3110400

int main(int argc, char *argv[])
{
    FILE *fp;

    fp = fopen("snap_mem.yuv", "wb");
    if (fp == NULL)
    {
        printf("ERR: open file\n");
        return -1;
    }

    uint8_t *img = malloc(sizeof(uint8_t) * IMGSIZE);

    if (snap_yuv_nv21_init(&img) != 0)
    {
        printf("err1\n");
        fclose(fp);
        free(img);
        return -1;
    }

    if (snap_yuv_nv21(&img) != 0)
    {
        printf("err2\n");
        fclose(fp);
        free(img);
        return -1;
    }

    if (snap_yuv_nv21_exit(&img) != 0)
    {
        printf("err3\n");
        fclose(fp);
        free(img);
        return -1;
    }

    fwrite(img, IMGSIZE, 1, fp);
    fclose(fp);
    free(img);

    return 0;
}
