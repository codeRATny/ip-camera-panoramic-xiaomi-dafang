#include "rgb24_jpg.h"

#define WIDTH 1920
#define HEIGHT 1080
#define FILENAME_IN "1.raw"
#define FILENAME_OUT "1.jpg"
#define QUALITY 80

int main()
{
    uint8_t* img = malloc(sizeof(uint8_t) * WIDTH * HEIGHT * PIXEL_SIZE);
    if (img == NULL) {
        printf("NULL pointer\n");
        exit(1);
    }

    FILE* fp = fopen(FILENAME_IN, "rb");
    fread(img, 1, WIDTH * HEIGHT * PIXEL_SIZE, fp);

    rgb24_to_jpeg(img, FILENAME_OUT, WIDTH, HEIGHT, QUALITY);

    free(img);
    fclose(fp);
    return 0;
}