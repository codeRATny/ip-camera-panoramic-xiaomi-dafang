#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <stdlib.h>

#include "raw_img.h"

#define IMGSIZE 1920 * 1080 * 2

int main(int argc, char *argv[])
{

    FILE *fp;

    fp = fopen("snap_mem.raw", "wb");
    if (fp == NULL)
    {
        printf("ERR: open file\n");
        return -1;
    }

    snap_raw_picture_file("snap_file.raw");
    uint8_t *img = malloc(sizeof(uint8_t) * IMGSIZE);

    if (snap_raw_picture_memory(&img) != 0)
    {
        fclose(fp);
        free(img);
        return -1;
    }

    fwrite(img, IMGSIZE, 1, fp);
    fclose(fp);
    free(img);

    return 0;
}