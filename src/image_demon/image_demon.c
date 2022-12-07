#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "head_motor.h"
#include "image_demon.h"
#include "snap_nv12/nv_12_img.h"
#include "nv12_to_rgb24/nv12_rgb24.h"
// #include "../picture_processing/image concatenation/img_concat.h"
#include "rgb24_to_jpeg/rgb24_jpg.h"

#define PIXEL_COLS 2
#define STEPS 2600
#define START_POS 300
#define QUALITY 80

int main()
{
    uint8_t* img_yuv = malloc(sizeof(uint8_t) * SENSOR_WIDTH * SENSOR_HEIGHT * 2);
    uint8_t* img_rgb = NULL;
    uint8_t* img_rgb_out = malloc(sizeof(uint8_t) * STEPS * PIXEL_COLS * SENSOR_HEIGHT);
    // uint8_t *img_out = NULL;

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
        // Test socket connection, should analyze commands
        char message[100];
        if (recv(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("recv");
            break;
        }
        strcpy(message, "Hello, world!");
        if (send(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("send");
            break;
        }
        // End of test connection

        // Пишем в mqueue команду демону мотора повернуться до упора влево

        if (init_queue() == -1)
        {
            break;
        }

        motor_action_e action = CAM2MOTOR_ACTION_CALIBRATION;

        if (send_command_to_motor(action) == -1)
        {
            break;
        }

        // Ждем подтверждения

        motor2pic_t reply;
        reply = receive_command_from_motor();

        if (reply.action_m2p == CAM2MOTOR_ACTION_INVALID_TYPE)
        {
            break;
        }

        // Делаем фотку, вырезаем столбец пикселей

        int img_counter = 0;

        do
        {
            // Пишем команду демону мотора повернуться на минимальный шаг вправо
            action = CAM2MOTOR_ACTION_STEP;
            if (send_command_to_motor(action) == -1)
            {
                return -1;
            }
            // Ждем подтверждения
            reply = receive_command_from_motor();
            if (reply.action_m2p == CAM2MOTOR_ACTION_INVALID_TYPE)
            {
                return -1;
            }

            int start_idx = START_POS * PIXEL_SIZE;
            int offset_idx = 0;

            // get new image here
            snap_yuv_nv21(&img_yuv);
            printf("Snap - ok\n");

            img_rgb = converter(img_yuv);
            printf("Convert to rgb - ok\n");

            for (int row = 0; row < SENSOR_HEIGHT; row++)
            {
                for (int col = 0; col < PIXEL_SIZE * PIXEL_COLS; col++)
                {
                    img_rgb_out[row * STEPS * (PIXEL_COLS * PIXEL_SIZE) + col + (PIXEL_COLS * img_counter * PIXEL_SIZE)] = img_rgb[start_idx + offset_idx + col];
                }
                offset_idx += SENSOR_WIDTH * PIXEL_SIZE;
            }
            img_counter++;
            // Если дошли до упора, кодируем в JPEG и отправляем телеграм боту, иначе делаем фотку и так по кругу
        } while (reply.action_m2p == CAM2MOTOR_ACTION_STEP);

        rgb24_to_jpeg(img_rgb_out, "/root/1.jpg", STEPS * PIXEL_COLS, SENSOR_HEIGHT, QUALITY);
        printf("Convert to jpg - ok\n");

        close_queue();
        close(client_fd);
    }
    free(img_rgb_out);
    free(img_yuv);
    close(camera_fd);
    return 0;
}