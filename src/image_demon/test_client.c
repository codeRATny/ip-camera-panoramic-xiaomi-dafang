#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(){
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd==-1){
        perror("socket");
        return -1;
    }
    struct sockaddr_in socket_info;
    memset(&socket_info,0,sizeof(socket_info));
    socket_info.sin_family=AF_INET;
    socket_info.sin_port=htons(5555);
    socket_info.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if(connect(client_fd,(struct sockaddr*)&socket_info,sizeof(socket_info))==-1){
        perror("connect");
        return -1;
    }
    char message[100];
    strcpy(message,"Hello, camera!");
    if(send(client_fd,message,sizeof(message),0)==-1){
        perror("send");
        return -1;
    }
    if(recv(client_fd,message,sizeof(message),0)==-1){
        perror("recv");
        return -1;
    }
    printf("%s\n",message);
    close(client_fd);
    return 0;
}