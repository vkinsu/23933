#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef struct Node {
	char* data;
	struct Node* next;
} Node;

Node* add_Node(Node* head, const char* str){
	Node *new_node = (Node *)malloc(sizeof(Node));
	
	new_node->data = (char *)malloc(strlen(str) + 1);
    if (new_node->data == NULL) {
        perror("Failed to allocate memory for string");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    
    strcpy(new_node->data, str);
    new_node->next = NULL;
    
    if (head == NULL) return new_node;
    
    Node* x = head;
    while (x->next != NULL) x = x->next;
    x->next = new_node;
    
    return head;
}

void free_list(Node* head){
	Node* x = head;
	while (x != NULL){
		Node* next = x->next;
		free(x->next);
		free(x);
		x = next;
	}
}

int main(){
    Node* list = NULL;
    char line[1024];
    while (1){
    
		if(fgets(line, sizeof(line), stdin) == NULL){
			perror("Error reading input");
			free_list(list);
			exit(EXIT_FAILURE);
		}
		if (line[0] == '.' && (line[1] == '\n' || line[1] == '\0')) break;
		
		list = add_Node(list, line);
    }
    printf("\nStrings:\n");
    Node* x = list;
    while (x != NULL){
    	printf("%s", x->data);
    	x = x->next;
    }
    free_list(list);
    return 0;
}
