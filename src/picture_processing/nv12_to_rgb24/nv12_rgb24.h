#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
// #include <x86intrin.h>

#define FIXED_POINT_VALUE(value, precision) ((int)(((value) * (1 << precision)) + 0.5))

typedef struct
{
    uint8_t cb_factor;
    uint8_t cr_factor;
    uint8_t g_cb_factor;
    uint8_t g_cr_factor;
    uint8_t y_factor;
    uint8_t y_offset;
} YUV2RGBParam;

uint8_t clamp(int16_t value);
void nv12_rgb24_std(
    uint32_t width, uint32_t height,
    const uint8_t *Y, const uint8_t *UV, uint32_t Y_stride, uint32_t UV_stride,
    uint8_t *RGB, uint32_t RGB_stride);
uint8_t *converter1(uint8_t *YUV);