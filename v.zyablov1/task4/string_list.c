#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

void append(Node **head, const char *str) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = (char *)malloc(strlen(str) + 1); // +1 для нулевого символа
    strcpy(new_node->data, str);
    new_node->next = *head; // Вставляем в начало списка
    *head = new_node;
}

void free_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
}

int main() {
    Node *head = NULL;
    char input[256]; // Размер массива для ввода строки

    printf("Введите строки (введите '.' для завершения):\n");
    while (1) {
        fgets(input, sizeof(input), stdin); // Читаем строку

        // Убираем символ новой строки
        input[strcspn(input, "\n")] = 0;

        // Проверяем, не введена ли точка
        if (strcmp(input, ".") == 0) {
            break;
        }

        append(&head, input);
    }

    // Выводим строки из списка
    printf("\nВведенные строки:\n");
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }

    // Освобождаем память
    free_list(head);

    return 0;
}

