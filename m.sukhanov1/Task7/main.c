#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

struct cell{
	int offset;
	int length;
};

void printall(int fd, char *mapping){
	int i = 0;
	char c;
	int size = lseek(fd, 0, SEEK_END);
	printf("\n");
	while(i < size){
		printf("%c", mapping[i++]);
	}
	printf("\n");
}

int f_in;
char* mapping;

void alarm_handler(int signum){
	printall(f_in, mapping);
	exit(0);
}

int main(){
	
	signal(SIGALRM, alarm_handler);
		
	if ((f_in = open("text.txt", O_RDONLY)) == -1){
		perror("Couldn't open the file!!!\n");
		return 2;
	} 
	char c;
	int count_of_lf = 0;
	int size = lseek(f_in, 0, SEEK_END);

	mapping = (char*)mmap(0, size, PROT_READ, MAP_SHARED, f_in, 0);

	int i = 0;
	
//	printf("file::\n");
	while(i < size){
		if (mapping[i] == '\n')
			count_of_lf++;
//		printf("%c", mapping[i]);
		i++;
	}
//	printf("END___\n\n\n\n");
	struct cell* Table = malloc(sizeof(struct cell) * (count_of_lf + 2));
	Table[0].offset = 0;
	Table[0].length = 0;
//	printf("%d is amount of \\n\n", count_of_lf);
	
	i = 0;
	int index = 0;
	while(i < size){
//		printf("%c", mapping[i]);
		if (mapping[i] == '\n'){
//			printf("current \\n is %d", i);
			Table[index+1].offset = i+1; 
			Table[index+1].length = Table[index+1].offset - Table[index].offset - 1;
//			printf("Table offset: %d ---- Table length: %d\n", Table[i].offset, Table[i].length);
			index++;
		}
		i++;
	}
//	printf("\n\n\n");
	Table[index+1].offset = size;
	Table[index+1].length = Table[index+1].offset - Table[index].offset;
	
/*
	for(int j =	1; j < count_of_lf+2; j++){
		printf("String number %d: offset=%d --- length=%d\n", j, Table[j].offset, Table[j].length);
	}
*/
	int num;
	char buffstring[256] = {0};
	
	while(1){
		printf("Enter a number of string: ");
		alarm(5);
		scanf("%d", &num);
		alarm(0);
		if (num == 0){
			break;
		}
		if (num > count_of_lf + 1){
			printf("There is no string with this number!\n");
			break;
		}
		int k = 0;
		for(int j = Table[num - 1].offset; j < Table[num - 1].offset + Table[num].length; j++){
			buffstring[k++] = mapping[j];
		}
		buffstring[k] = '\0';
		printf("%s\n", buffstring);
	}
	free(Table);
	close(f_in);
	return 0;
}
