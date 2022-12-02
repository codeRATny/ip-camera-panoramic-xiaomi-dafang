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

int init_queue(){
    mqueue.pic2motor_queue = mq_open(PIC2MOTOR_QUEUE, O_WRONLY | O_CREAT, S_IWUSR, &attributes_for_motor_queue);
    if(mqueue.pic2motor_queue==-1){
        perror("mq_open pic2motor");
        return -1;
    }
    mqueue.motor2pic_queue = mq_open(MOTOR2PIC_QUEUE, O_RDONLY | O_CREAT, S_IWUSR, &attributes_for_motor_queue);
    if(mqueue.pic2motor_queue==-1){
        perror("mq_open pic2motor");
        return -1;
    }
    return 0;
}

int send_command_to_motor(motor_action_t action){
    if(mq_send(mqueue.pic2motor_queue, (const char*)&action, sizeof(action), PRIORITY_OF_QUEUE)==-1){
        perror("mq_send to motor");
        return -1;
    }
    printf("Command sent\n");
    return 0;
}

motor_action_t receive_command_from_motor(){
    motor_action_t action;
    if(mq_receive(mqueue.motor2pic_queue,(char*)&action,sizeof(action),NULL)==-1){
        perror("mq_receive from motor");
        return -1;
    }
    printf("Command received\n");
    return action;
}

void close_queue(){
    mq_close(mqueue.pic2motor_queue);
    mq_close(mqueue.motor2pic_queue);
}

int main(){
    // Create socket for incoming connections
    int camera_fd=socket(AF_INET,SOCK_STREAM,0);
    if(camera_fd==-1){
        perror("socket");
        return -1;
    }
    // Binding port
    struct sockaddr_in socket_info;
    memset(&socket_info,0,sizeof(socket_info));
    socket_info.sin_family=AF_INET;
    socket_info.sin_port=htons(PORT);
    if(bind(camera_fd,(struct sockaddr*)&socket_info,sizeof(socket_info))==-1){
        perror("bind");
        return -1;
    }
    // Listening port
    if(listen(camera_fd,5)==-1){
        perror("listen");
        return -1;
    }
    while(1){
        // Пока план такой:

        // Получаем команду из телеги делать красиво

        // Accept connection
        struct sockaddr_in client;
        socklen_t client_size=sizeof(client);
        int client_fd=accept(camera_fd,(struct sockaddr*)&client,&client_size);
        if(client_fd==-1){
            perror("accept");
            return -1;
        }
        // Test socket connection, should analyze commands
        char message[100];
        if(recv(client_fd,message,sizeof(message),0)==-1){
            perror("recv");
            return -1;
        }
        printf("Command received: %s\n",message);
        strcpy(message,"Hello, world!");
        if(send(client_fd,message,sizeof(message),0)==-1){
            perror("send");
            return -1;
        }
        printf("Reply sent\n");
        // End of test connection

        // Пишем в mqueue команду демону мотора повернуться до упора влево

        if(init_queue()==-1){
            return -1;
        }

        motor_action_t action = CAM2MOTOR_ACTION_CALIBR;

        if(send_command_to_motor(action)==-1){
            return -1;
        }

        // Ждем подтверждения

        action = receive_command_from_motor();

        if(action == CAM2MOTOR_ACTION_INVALID_TYPE){
            printf("Motor error\n");
            return -1;
        }        
        // Делаем фотку, вырезаем столбец пикселей
        int i=0;
        while(i<20){
            // Пишем команду демону мотора повернуться на минимальный шаг вправо
            action = CAM2MOTOR_ACTION_STEP;
            if(send_command_to_motor(action)==-1){
                return -1;
            }
            // Ждем подтверждения
            action=receive_command_from_motor();
            if(action==CAM2MOTOR_ACTION_INVALID_TYPE){
                printf("Motor error\n");
                break;
            }
            // Если дошли до упора, кодируем в JPEG и отправляем телеграм боту, иначе делаем фотку и так по кругу
            else if(action==CAM2MOTOR_ACTION_END_OF_ENUM){
                printf("End of path reached\n");
                break;
            }
            i++;
        }
        close_queue();
        close(client_fd);
    }
    close(camera_fd);
    return 0;
}