#include "nv12_rgb24.h"

int main()
{
    FILE *fd_in = fopen("1.yuv", "rb");
    FILE *fd_out = fopen("1.raw", "wb");
    uint8_t *img_in = malloc(3110400);
    uint8_t *img_out = NULL;
    fread(img_in, 1, 3110400, fd_in);
    img_out = converter1(img_in);
    fwrite(img_out, 1, 3 * 1920 * 1080, fd_out);

    free(img_in);
    free(img_out);
    fclose(fd_in);
    fclose(fd_out);
    return 0;
}