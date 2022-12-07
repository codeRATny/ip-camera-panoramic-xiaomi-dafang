#include "nv12_rgb24.h"

static const YUV2RGBParam YUV2RGB1 = {
    .cb_factor = FIXED_POINT_VALUE(255.0 * (2.0 * (1 - 0.114)) / 224.0, 6),
    .cr_factor = FIXED_POINT_VALUE(255.0 * (2.0 * (1 - 0.299)) / 224.0, 6),
    .g_cb_factor = FIXED_POINT_VALUE(0.114 / (1.0 - 0.114 - 0.299) * 255.0 * (2.0 * (1 - 0.114)) / 224.0, 7),
    .g_cr_factor = FIXED_POINT_VALUE(0.299 / (1.0 - 0.114 - 0.299) * 255.0 * (2.0 * (1 - 0.114)) / 224.0, 7),
    .y_factor = FIXED_POINT_VALUE(255.0 / (235.0 - 16.0), 7),
    .y_offset = 16.0};

uint8_t clamp(int16_t value)
{
    return value < 0 ? 0 : (value > 255 ? 255 : value);
}

void nv12_rgb24_std(
    uint32_t width, uint32_t height,
    const uint8_t *Y, const uint8_t *UV, uint32_t Y_stride, uint32_t UV_stride,
    uint8_t *RGB, uint32_t RGB_stride)
{
    const YUV2RGBParam *const param = &(YUV2RGB1);
    uint32_t x, y;
    for (y = 0; y < (height - 1); y += 2)
    {
        const uint8_t *y_ptr1 = Y + y * Y_stride,
                      *y_ptr2 = Y + (y + 1) * Y_stride,
                      *uv_ptr = UV + (y / 2) * UV_stride;

        uint8_t *rgb_ptr1 = RGB + y * RGB_stride,
                *rgb_ptr2 = RGB + (y + 1) * RGB_stride;

        for (x = 0; x < (width - 1); x += 2)
        {
            int8_t u_tmp, v_tmp;
            u_tmp = uv_ptr[0] - 128;
            v_tmp = uv_ptr[1] - 128;

            int16_t b_cb_offset, r_cr_offset, g_cbcr_offset;
            b_cb_offset = (param->cb_factor * u_tmp) >> 6;
            r_cr_offset = (param->cr_factor * v_tmp) >> 6;
            g_cbcr_offset = (param->g_cb_factor * u_tmp + param->g_cr_factor * v_tmp) >> 7;

            int16_t y_tmp;
            y_tmp = (param->y_factor * (y_ptr1[0] - param->y_offset)) >> 7;
            rgb_ptr1[0] = clamp(y_tmp + r_cr_offset);
            rgb_ptr1[1] = clamp(y_tmp - g_cbcr_offset);
            rgb_ptr1[2] = clamp(y_tmp + b_cb_offset);

            y_tmp = (param->y_factor * (y_ptr1[1] - param->y_offset)) >> 7;
            rgb_ptr1[3] = clamp(y_tmp + r_cr_offset);
            rgb_ptr1[4] = clamp(y_tmp - g_cbcr_offset);
            rgb_ptr1[5] = clamp(y_tmp + b_cb_offset);

            y_tmp = (param->y_factor * (y_ptr2[0] - param->y_offset)) >> 7;
            rgb_ptr2[0] = clamp(y_tmp + r_cr_offset);
            rgb_ptr2[1] = clamp(y_tmp - g_cbcr_offset);
            rgb_ptr2[2] = clamp(y_tmp + b_cb_offset);

            y_tmp = (param->y_factor * (y_ptr2[1] - param->y_offset)) >> 7;
            rgb_ptr2[3] = clamp(y_tmp + r_cr_offset);
            rgb_ptr2[4] = clamp(y_tmp - g_cbcr_offset);
            rgb_ptr2[5] = clamp(y_tmp + b_cb_offset);

            rgb_ptr1 += 6;
            rgb_ptr2 += 6;
            y_ptr1 += 2;
            y_ptr2 += 2;
            uv_ptr += 2;
        }
    }
}

uint8_t *converter1(uint8_t *YUV)
{
    uint8_t *RGB = NULL, *Y = NULL, *U = NULL, *RGBa = NULL, *YUVa = NULL, *Ya = NULL, *Ua = NULL;
    uint32_t width, height;
    width = 1920;
    height = 1080;

    RGB = malloc(3 * width * height);

    Y = YUV;
    U = YUV + width * height;

    const size_t y_stride = width + (16 - width % 16) % 16;
    const size_t uv_stride = y_stride;
    const size_t rgb_stride = width * 3 + (16 - (3 * width) % 16) % 16;

    const size_t y_size = y_stride * height, uv_size = uv_stride * ((height + 1) / 2);
    // YUVa = _mm_malloc(y_size + 2 * uv_size, 16);
    YUVa = memalign(16, y_size + 2 * uv_size);
    // posix_memalign(&YUVa, 16, y_size + 2 * uv_size);

    Ya = YUVa;
    Ua = YUVa + y_size;
    for (unsigned int i = 0; i < height; ++i)
    {
        memcpy(Ya + i * y_stride, Y + i * width, width);
        if ((i % 2) == 0)
        {
            memcpy(Ua + (i / 2) * uv_stride, U + (i / 2) * width, width);
        }
    }

    // RGBa = _mm_malloc(rgb_stride * height, 16);
    RGBa = memalign(16, rgb_stride * height);
    // posix_memalign(&RGBa, 16, rgb_stride * height);

    Y = YUV;
    U = YUV + width * height;

    nv12_rgb24_std(width, height, Y, U, width, width, RGB, width * 3);

    free(RGBa);
    free(YUVa);
    return RGB;
}