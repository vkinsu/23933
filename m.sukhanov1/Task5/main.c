#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

struct cell{
	int offset;
	int length;
};

void printall(int fd){
	lseek(fd, 0, SEEK_SET);
	char c;
	while(read(fd, &c, 1)){
		printf("%c", c);
	}
}

int main(){
	
	int f_in;
	if ((f_in = open("text.txt", O_RDONLY)) == -1){
		perror("Couldn't open the file!!!\n");
		return 2;
	} 
	char c;
	int count_of_lf = 0;
	
	while(read(f_in, &c, 1)){
		if (c == '\n')
			count_of_lf++;
	}
	struct cell* Table = malloc(sizeof(struct cell) * (count_of_lf + 2));
	Table[0].offset = 0;
	Table[0].length = 0;
	lseek(f_in, 0, SEEK_SET);
	
	int i = 1;
	int index = 0;
	//printf("First value of char: %c\n", c);
	//printf("First value of lseek: %d\n", lseek(f_in, 0, SEEK_CUR));
	while(read(f_in, &c, 1)){
		//printf("%d", c);
		//printf("%c %d\n", c, index++);
		if (c == '\n'){
			//printf("Number: %d\n", i);			
			Table[i].offset = lseek(f_in, 0, SEEK_CUR); 
			//printf("OFFSET: %d\n", Table[i].offset);
			Table[i].length = Table[i].offset - Table[i-1].offset - 1;
			//printf("LENGTH: %d\n", Table[i].length);			
			i++;
		}
	}
	//printf("\n");
	Table[i].offset = lseek(f_in, 0, SEEK_CUR) + 1;
	Table[i].length = Table[i].offset - Table[i-1].offset - 1;
	//printf("Number: %d\n", i);
	//printf("OFFSET: %d\n", Table[i].offset);
	//printf("LENGTH: %d\n", Table[i].length);
	//printf("\n");
	//printf("++++++++++++++++++++\n\tOTHER PART\n++++++++++++++++++++\n");
	
	int num;
	char buffstring[256] = {0};
	/*for (i = 0; i < count_of_lf + 2; i++){
		//printf("NUMBER: %d \n", i);
		//printf("OFFSET:= %d\n", Table[i].offset);
		//printf("LENGTH:= %d\n", Table[i].length);
		//printf("\n\n");
	}*/
	
	
	while(1){
		printf("Enter a number of string: ");
		scanf("%d", &num);
		if (num == 0){
			//printall(f_in);
			break;
		}
		if (num > count_of_lf + 1){
			printf("There is no string with this number!\n");
			break;
		}
		lseek(f_in, Table[num - 1].offset, SEEK_SET);
		read(f_in, buffstring, Table[num].length);
		buffstring[Table[num].length] = '\0';
		printf("%s\n", buffstring);
	}
	free(Table);
	close(f_in);
	return 0;
}