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

    if(listen(fd, 3) == -1){
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
        return 0;
    }
    else{
        int stat;
        wait(&stat);
        if(stat != 0){
            printf("Child process has returned %d\n", stat);
            close(fd);
            return stat;
        }
        
        struct pollfd fds;
        fds.fd = fd;
        fds.events = POLLIN;

        printf("\nServer output:\n");

        int connects = 0;
        while(connects < 3){
            int client_fd = accept(fd, NULL, NULL);
            if(client_fd == -1){
                perror("Accept error");
                connects++;
                continue;
            }

            pid_t pid = fork();
            if(pid == -1){
                perror("Can't create child process");
                close(fd);
                return -1;
            }

            if(pid == 0){
                int rc;
                char sym;
                while((rc = read(client_fd, &sym, 1)) > 0){
                    putchar(toupper(sym));
                }
                if(rc == -1){
                    perror("Read error");
                    exit(-1);
                }
                else if (rc == 0) {
                    close(client_fd);
                }
                putchar('\n');
                return 0;
            }
            connects++;
        }
    }    
    close(fd);
    return 0;
}