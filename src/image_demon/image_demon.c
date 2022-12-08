#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "snap_nv12/nv_12_img.h"
#include "nv12_to_rgb24/nv12_rgb24.h"
#include "rgb24_to_jpeg/rgb24_jpg.h"
#include "motor_daemon_api.h"
#include "motor_daemon.h"
#include <malloc.h>
#include <time.h>
#include <execinfo.h>

#define PIXEL_COLS 2
#define START_POS 300
#define QUALITY 80
#define PORT 5555

void handler(int sig)
{
    void *array[10];
    size_t size;

    size = backtrace(array, 10);

    fprintf(stdout, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDOUT_FILENO);
    exit(1);
}

int main()
{
    signal(SIGSEGV, handler); // install our handler
    uint8_t *img_yuv = malloc(sizeof(uint8_t) * SENSOR_WIDTH * SENSOR_HEIGHT * 2);

    if (img_yuv == NULL)
    {
        printf("NULL1\n");
        return -1;
    }

    uint8_t *img_rgb = NULL;
    uint8_t *img_rgb_out = malloc(sizeof(uint8_t) * LIMIT_STEP * PIXEL_COLS * SENSOR_HEIGHT * PIXEL_SIZE);
    if (img_rgb_out == NULL)
    {
        printf("NULL1\n");
        return -1;
    }

    // Create socket for incoming connections
    int camera_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (camera_fd == -1)
    {
        perror("socket");
        return -1;
    }

    // Binding port
    struct sockaddr_in socket_info;
    memset(&socket_info, 0, sizeof(socket_info));
    socket_info.sin_family = AF_INET;
    socket_info.sin_port = htons(PORT);

    if (bind(camera_fd, (struct sockaddr *)&socket_info, sizeof(socket_info)) == -1)
    {
        perror("bind");
        return -1;
    }

    // Listening port
    if (listen(camera_fd, 5) == -1)
    {
        perror("listen");
        return -1;
    }

    while (1)
    {
        // Пока план такой:

        // Получаем команду из телеги делать красиво

        // Accept connection
        struct sockaddr_in client;
        socklen_t client_size = sizeof(client);
        int client_fd = accept(camera_fd, (struct sockaddr *)&client, &client_size);
        if (client_fd == -1)
        {
            perror("accept");
            break;
        }
        char message[100];
        if (recv(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("recv");
            break;
        }

        // Пишем в mqueue команду демону мотора повернуться до упора влево

        init_contol_motor();

        calibration();

        // Ждем подтверждения

        // Делаем фотку, вырезаем столбец пикселей

        int img_counter = 0;

        int ret = 0;

        snap_yuv_nv21_init();
        // printf("init - ok\n");

        do
        {
            ret = step(1);
            // printf("Cycle - ok\n");

            int start_idx = START_POS * PIXEL_SIZE;
            int offset_idx = 0;

            snap_yuv_nv21(&img_yuv);
            // printf("Snap - ok\n");

            img_rgb = converter1(img_yuv);
            // printf("Convert to rgb - ok\n");

            for (int row = 0; row < SENSOR_HEIGHT; row++)
            {
                for (int col = 0; col < PIXEL_SIZE * PIXEL_COLS; col++)
                {
                    img_rgb_out[row * LIMIT_STEP * (PIXEL_COLS * PIXEL_SIZE) + col + (PIXEL_COLS * img_counter * PIXEL_SIZE)] = img_rgb[start_idx + offset_idx + col];
                }
                offset_idx += SENSOR_WIDTH * PIXEL_SIZE;
            }
            // printf("Write col - ok\n");
            img_counter++;
            free(img_rgb);
        } while (ret != CAM2MOTOR_ACTION_END_OF_ENUM);

        snap_yuv_nv21_exit();
        rgb24_to_jpeg(img_rgb_out, "/root/1.jpg", LIMIT_STEP * PIXEL_COLS, SENSOR_HEIGHT, QUALITY);
        printf("Convert to jpg - ok\n");
        
        strcpy(message, "/root/1.jpg");
        if (send(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("send");
            break;
        }

        close(client_fd);
    }
    free(img_rgb_out);
    free(img_yuv);
    close(camera_fd);
    return 0;
}
