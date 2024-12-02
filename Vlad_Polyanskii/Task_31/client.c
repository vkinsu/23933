#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void close_fds(struct pollfd* fds, int n){
    for(int i = 0; i < n; i++){
        close(fds[i].fd);
    }
}

void make_connects(struct pollfd* fds, int n){
    int fd;
    struct sockaddr_un addr;
    char* socket_path = "socket";

    for(int i = 0; i < n; i++){
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

        if(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
            perror("Connect error");
            close_fds(fds, i + 1);
            exit(1);
        }

        fds[i].fd = fd;
        fds[i].events = POLLOUT;
    }
}

int send_message(int fd, char* message){
    if(write(fd, message, strlen(message)) <= 0){
        printf("Can't send a message\n");
        return 1;
    }
    close(fd);
    return 0;
}

int main(){
    int n = 3;
    struct pollfd fds[3];
    make_connects(fds, n);
    
    int connects = 0;
    while(connects < 3){
        int ret = poll(fds, n, 10000);
        switch (ret)
        {
            case -1:{
                perror("Poll error");
                close_fds(fds, n);
                return 1;
                break;
            }
            case 0:{
                printf("Timeout\n");
                close_fds(fds, n);
                return 1;
                break;
            }
            default:{
                if(fds[0].revents & POLLOUT){
                    char message[256] = "Hello world!";
                    if(send_message(fds[0].fd, message) == 1){
                        close_fds(fds, n);
                        return 1;
                    }
                    printf("Client 0 send a message\n");
                }
                else if(fds[1].revents & POLLOUT){
                    char message[256] = "Other client";
                    if(send_message(fds[1].fd, message) == 1){
                        close_fds(fds, n);
                        return 1;
                    }
                    printf("Client 1 send a message\n");
                }
                else if(fds[2].revents & POLLOUT){
                    char message[256] = "Last message";
                    if(send_message(fds[2].fd, message) == 1){
                        close_fds(fds, n);
                        return 1;
                    }
                    printf("Client 2 send a message\n");
                }
                connects++;
                break;
            }
        }
    }
    return 0;
}