#include "img_concat.h"

int main()
{
    uint8_t* img = NULL;
    img = make_panorama_rgb24(1920, 1080, 1000, 2, 300);

    FILE* fd = fopen("5.raw", "wb");

    fwrite(img, 1, 1080 * 1000 * 2 * 3, fd);
    fclose(fd);
    free(img);

    return 0;
}