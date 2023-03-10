#ifndef _RGB24_JPG_H_
#define _RGB24_JPG_H_

#include "jpeglib/jpeglib.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>

#define PIXEL_SIZE 3

void rgb24_to_jpeg(uint8_t* img, const char* filename, int width, int height, int quality);

#endif