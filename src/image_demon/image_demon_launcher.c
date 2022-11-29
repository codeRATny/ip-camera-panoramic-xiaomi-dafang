#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(){
    pid_t pid=fork();
    if(pid==0){
        if(execl("image_demon","",NULL)==-1){
            perror("exec");
            return -1;
        }
    }
    else{
        printf("Demon PID = %d\n",pid);
    }
    return 0;
}