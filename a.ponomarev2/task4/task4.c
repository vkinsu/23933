#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *data;
    struct Node *next;
};

void insert(struct Node **head, const char *str) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = (char *)malloc(strlen(str) + 1);
    
    strcpy(newNode->data, str); 
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        struct Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void printList(struct Node *head) {
    struct Node *current = head;
    
    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;
    }
}


void freeList(struct Node *head) {
    while (head != NULL) {
        struct Node *temp = head;
        head = head->next;
        free(temp->data);
        free(temp);
    }
}

int main() {
    struct Node *head = NULL;
    char input[256];
    
    while (1) {
        fgets(input, sizeof(input), stdin);
        int len = strlen(input);
        if (len > 0 && input[0] == '.') break;

        insert(&head, input);
    }

    printf("\nInput:\n");
    printList(head);
    freeList(head);
    return 0;
}
