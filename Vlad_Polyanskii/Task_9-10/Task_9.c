#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE* child_proc = popen("cat test.txt", "w");
    if (child_proc == NULL){
        perror("Can't create child process");
        return 1;
    }
    wait(NULL);
    printf("\nParent message\n");
    pclose(child_proc);
    return 0;
}
