#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct Node {
    char *string;
    struct Node *next;
} Node;


int main() {
    char buf[1024];
    unsigned long str_len;

    Node *cur = malloc(sizeof(Node));
    cur->next = NULL;

    Node *first = cur;

//    Node *first = (Node *) &cur;

    while (fgets(buf, sizeof(buf), stdin) != 0) {
        str_len = strlen(buf);

        if (str_len >= 1 && buf[0] == '.') {
            break;
        }

        char *str = malloc(sizeof(char) * (str_len + 1));
        memcpy(str, buf, str_len + 1);
        cur->string = str;

        Node *next = malloc(sizeof(Node));
        next->next = NULL;

        cur->next = next;
        cur = next;

    }

    while (first->next != NULL) {
        printf("%s", first->string);
        free(first->string);
        Node *n = first->next;
        free(first);
        first = n;
    }

    return 0;
}
