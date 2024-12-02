#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char** argv){
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd == -1){
        perror("Socket error");
        return 1;
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    char* socket_path = "socket";
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);

    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("Bind error");
        close(fd);
        return 1;
    } 

    if(listen(fd, 1) == -1){
        perror("Listen error");
        close(fd);
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("Can't create child process");
        close(fd);
        return -1;
    }

    if(pid == 0){
        if(execvp("./client", argv) == -1){
            perror("execvp error");
            return 1;
        }
    }
    else{
        wait(NULL);
        int client_fd = accept(fd, NULL, NULL);
        if(client_fd == -1){
            perror("Accept error");
            return 1;
        }

        struct pollfd fds;
        fds.fd = client_fd;
        fds.events = POLLIN;
        
        int ret = poll(&fds, 1, 10000);
        switch (ret)
        {
            case -1:{
                perror("Poll error");
                close(fd);
                return 1;
                break;
            }
            case 0:{
                printf("Timeout\n");
                break;
            }
            default:{
                if(fds.revents & POLLIN){
                    fds.revents = 0;
                    char sym;
                    
                    printf("Server output: ");
                    while(read(client_fd, &sym, 1) > 0){
                        putchar(toupper(sym));
                    }
                    putchar('\n');
                }
                break;
            }
        }
        close(fd);
    }
    
    return 0;
}