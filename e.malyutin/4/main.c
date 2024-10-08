#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *content;
    struct Item *next;
} Item;

int main() {
    Item *first = NULL;
    Item *current = NULL;

    char buf[2048];

    while (1) {
        fgets(buf, sizeof buf, stdin);
        size_t len = strlen(buf);

        if (len > 0 && buf[0] == '.') {
            break;
        }

        char *content = malloc(len + 1);
        memcpy(content, buf, len + 1);

        Item *next = malloc(sizeof(Item));
        next->content = content;
        next->next = NULL;

        if (current == NULL) {
            current = next;
            first = next;
        } else {
            current->next = next;
            current = next;
        }
    }

    while (first) {
        printf("%s", first->content);
        Item *next = first->next;
        free(first->content);
        free(first);
        first = next;
    }
}