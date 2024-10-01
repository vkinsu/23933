#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node_t {
  char* line;
  struct Node_t* next;
} Node;

void clear(Node* node) {
  if (node == NULL) return;
  clear(node->next);
  if (node->line != NULL) free(node->line);
  if (node->next != NULL) free(node->next);
}

int main() {

  Node* my_lst = (Node*)malloc(sizeof(Node));
  my_lst->line = NULL;
  my_lst->next = NULL;

  Node* ptr = my_lst;

  char c;
  int size = 1;
  while ((c = getchar()) != EOF) {
    if (ptr->line == NULL && c == '.') break;
    if (c == '\n') {
      size = 1;
      ptr->next = (Node*)malloc(sizeof(Node));
      ptr = ptr->next;
      ptr->next = NULL;
      ptr->line = NULL;
    }
    else {
      if (ptr->line == NULL) ptr->line = (char*)malloc(sizeof(char));
      ptr->line = (char*)realloc(ptr->line, (++size) * sizeof(char));
      ptr->line[size - 2] = c;
    }
  }
  getchar();

  int i = 0;
  printf("Strings:\n");
  for (Node* node = my_lst; node != NULL; node = node->next) {
    if (node->line != NULL) printf("%d) %s\n", ++i, node->line);
    else printf("%d) \n", ++i);
  }

  clear(my_lst);
  free(my_lst);

  return 0;
}
