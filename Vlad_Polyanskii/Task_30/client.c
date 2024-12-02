#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    char* socket_path = "socket";
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) -1);

    if(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("Connect error");
        close(fd);
        return 1;
    }

    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLOUT;
    
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
            if(fds.revents & POLLOUT){
                fds.revents = 0;
                char message[256];
                sprintf(message, "Hello World!");
                if(write(fd, message, 256) <= 0){
                    printf("Can't send a message\n");
                    close(fd);
                    return 1;
                }
                printf("Client send a message\n");
                
            }
            break;
        }
    }
    close(fd);
    return 0;
}