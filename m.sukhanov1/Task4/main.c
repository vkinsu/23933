#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STR_SIZE 100

typedef struct Node{
	char *string;
	struct Node* next;
} Node;

int main(){

	char buff[MAX_STR_SIZE] = {};

	Node *head = malloc(sizeof(Node));
	head->next = NULL;
	Node *currNode = head;

	while(fgets(buff, MAX_STR_SIZE, stdin) && buff[0] != '.'){
		buff[strcspn(buff, "\n")] = '\0';
		currNode->string = malloc(sizeof(char) * strlen(buff) + 1);
		strcpy(currNode->string, buff);
		currNode->next = malloc(sizeof(Node));
		currNode = currNode->next;
		currNode->next = NULL;
	}
	

	printf("А вот и наши строки:\n");
	
	currNode = head;
	Node *temp;

	while(currNode->next){
		printf("%s\n", currNode->string);

		free(currNode->string);
		temp = currNode;
		
		currNode = currNode->next;
		free(temp);
	}
	free(currNode);

	exit(0);

}

