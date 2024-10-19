#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <termios.h>


//Change termios settings to not display user entered text and adjust recover default settings at exit
struct termios original_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enable_raw_mode();

    int pipe_fd[2];
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(2);
    }

    if (child_pid == 0) {
        close(pipe_fd[1]);
        signal(SIGQUIT, exit);

        char buffer;
        while (read(pipe_fd[0], &buffer, 1) > 0) {
            if (buffer == '\0') {
                break;
            }
            char uppercase = toupper(buffer);
            write(STDOUT_FILENO, &uppercase, 1);
        }

        close(pipe_fd[0]);
        printf("Child process finished\n");
//        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fd[0]);
        signal(SIGQUIT, exit);

        char input;
        while (read(STDIN_FILENO, &input, 1) > 0) {
            write(pipe_fd[1], &input, 1);
        }
//
//        close(pipe_fd[1]);
//        kill(child_pid, SIGQUIT);
//        wait(NULL);
//        exit(EXIT_SUCCESS);
    }
}
