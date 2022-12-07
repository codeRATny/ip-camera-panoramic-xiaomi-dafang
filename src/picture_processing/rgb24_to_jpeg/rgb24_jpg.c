#include "rgb24_jpg.h"

void rgb24_to_jpeg(uint8_t* img, const char* filename, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* img_jpg;
    if ((img_jpg = fopen(filename, "wb")) == NULL) {
        printf("can't open %s\n", filename);
        // return -1;
    }

    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, img_jpg);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = PIXEL_SIZE;
    cinfo.in_color_space = JCS_RGB; 

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = width * PIXEL_SIZE;

    while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &img[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);
    fclose(img_jpg);
}