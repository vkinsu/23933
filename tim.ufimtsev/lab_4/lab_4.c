#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *data;
    struct Node *next;
};

void addNode(struct Node **head, const char *str) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = (char *)malloc(strlen(str) + 1);
    strcpy(newNode->data, str);
    newNode->next = *head;
    *head = newNode;
}

void printList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;
    }
}

void freeList(struct Node *head) {
    struct Node *current = head;
    struct Node *next;
    while (current != NULL) {
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}

int main() {
    struct Node *head = NULL;
    char buffer[1024];

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        if (buffer[0] == '.') {
            break;
        }
        addNode(&head, buffer);
    }

    printList(head);

    freeList(head);

    return 0;
}
