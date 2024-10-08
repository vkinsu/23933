#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define _size 1024

typedef struct Nodes{
  char *string;
  struct node* next;
} Node;

Node *head, *tail;

void pushstr(char* str){
  unsigned long len = strlen(str) + 1;
  char *copyPtr = calloc(len, sizeof(char));
  strcpy(copyPtr, str);

  tail->string = copyPtr;
  tail->next = calloc(1, sizeof(Node));

  tail = tail->next;
}

int main(){
  char str[_size];

  head = malloc(sizeof(Node));
  head->string = NULL;
  head->next = NULL;
  tail = head;

  while (fgets(str, _size, stdin) != NULL) {
        if (str[0] == '.') {
          pushstr(str + 1);
          //printList
          printf("\nYour list:\n");
          Node *ptr = head;
          while (ptr != NULL) {
              if (ptr->string) {
                  printf("%s\n", ptr->string);
              }
              ptr = ptr->next;
          }
          return 0;
        }

        char *lineEnd = strchr(str, '\n');
        if (lineEnd == NULL) {
            size_t newBufCnt = 0;
            size_t newBufCap = _size;
            char *newBuf = malloc(_size);

            memcpy(newBuf, str, _size);
            newBufCnt += _size - 1;

            while (fgets(str, _size, stdin) != NULL) {
                newBufCap += _size;
                newBuf = realloc(newBuf, newBufCap);

                memcpy(newBuf + newBufCnt, str, _size);
                newBufCnt += _size - 1;

                lineEnd = strchr(newBuf, '\n');
                if (lineEnd) {
                    *lineEnd = '\0';
                    pushstr(newBuf);
                    free(newBuf);
                    break;
                }
            }
        } else {
            *lineEnd = '\0';
            pushstr(str);
        }
    }


  return 0;
}
