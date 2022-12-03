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

int main()
{
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
            return -1;
        }
        // Test socket connection, should analyze commands
        char message[100];
        if (recv(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("recv");
            return -1;
        }
        strcpy(message, "Hello, world!");
        if (send(client_fd, message, sizeof(message), 0) == -1)
        {
            perror("send");
            return -1;
        }
        // End of test connection

        // Пишем в mqueue команду демону мотора повернуться до упора влево

        if (init_queue() == -1)
        {
            return -1;
        }

        motor_action_e action = CAM2MOTOR_ACTION_CALIBRATION;

        if (send_command_to_motor(action) == -1)
        {
            return -1;
        }

        // Ждем подтверждения

        motor2pic_t reply;
        reply = receive_command_from_motor();

        if (reply.action_m2p == CAM2MOTOR_ACTION_INVALID_TYPE)
        {
            return -1;
        }
        // Делаем фотку, вырезаем столбец пикселей
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
            // Если дошли до упора, кодируем в JPEG и отправляем телеграм боту, иначе делаем фотку и так по кругу
        } while (reply.action_m2p == CAM2MOTOR_ACTION_STEP);
        close_queue();
        close(client_fd);
    }
    close(camera_fd);
    return 0;
}