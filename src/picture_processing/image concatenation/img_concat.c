#include "img_concat.h"

uint8_t *make_panorama_rgb24(int img_width, int img_height, int steps, int pixel_cols, int start_pos)
{
    uint8_t *img_out_buf = malloc(sizeof(uint8_t) * pixel_cols * steps * PIXEL_SIZE * img_height);
    if (img_out_buf == NULL)
    {
        printf("NULL\n");
        exit(1);
    }

    uint8_t *buf = malloc(sizeof(uint8_t) * img_width * img_height * PIXEL_SIZE);
    if (img_out_buf == NULL)
    {
        printf("NULL\n");
        exit(1);
    }

    // FOR E.G.
    int fd_img_in;
    fd_img_in = open("1.raw", O_RDONLY);
    read(fd_img_in, buf, img_width * img_height * PIXEL_SIZE);

    for (int img_counter = 0; img_counter < steps; img_counter++)
    {
        int start_idx = start_pos * PIXEL_SIZE;
        int offset_idx = 0;

        // get new image here
        // snap_yuv_nv21(&buf);
        for (int row = 0; row < img_height; row++)
        {
            for (int col = 0; col < PIXEL_SIZE * pixel_cols; col++)
            {
                img_out_buf[row * steps * (pixel_cols * PIXEL_SIZE) + col + (pixel_cols * img_counter * PIXEL_SIZE)] = buf[start_idx + offset_idx + col];
            }
            offset_idx += img_width * PIXEL_SIZE;
        }
    }

    free(buf);
    close(fd_img_in);

    return img_out_buf;
}