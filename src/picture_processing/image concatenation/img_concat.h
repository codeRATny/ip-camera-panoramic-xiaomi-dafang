#ifndef _IMG_CONCAT_H_
#define _IMG_CONCAT_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>

#define PIXEL_SIZE 3

uint8_t* make_panorama_rgb24(int img_width, int img_height, int steps, int pixel_cols, int start_pos);

#endif