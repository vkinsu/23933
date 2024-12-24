#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#define S_LENGTH 40
#define ERASE 127
#define KILL 21
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7

void erase_char(int *pos, char *str){
	if ((*pos) == 0){
		putchar('\a');
	}
	else{
		str[*pos] = '\0';
		write(fileno(stdout), "\b \b", 3);
	}	
}

int main(){
	char ch, buf_str[S_LENGTH+1] = {0};

	struct termios tty, savtty;

	int space_cnt = 0, pos, fd = open("/dev/tty", O_RDONLY);

	pos = 0;

	tcgetattr(fd, &tty);

	if (isatty(fileno(stdout)) == 0){
		fprintf(stderr, "stdout isn't a teminal\n");
		exit(1);
	}

	savtty = tty;

	tty.c_lflag &= ~(ICANON | ECHO);

	tty.c_cc[VMIN] = 1;

	tcsetattr(fd, TCSAFLUSH, &tty);

	setbuf(stdout, (char *) NULL);

	while(1){
		read(fileno(stdin), &ch, 1);
		if (ch == CTRL_D && pos == 0)
			break;
		if (ch == ' ')
			++space_cnt;
		buf_str[pos] = ch;
		pos++;
		if (ch =='\n'){
			pos = 0;
			space_cnt = 0;
		}
		if (ch == ERASE){
			erase_char(&pos, buf_str);
			buf_str[pos] = '\0';
			pos--;
		}
		if (ch == KILL){
			write(fileno(stdout), "\r", 1);
			for(int j = 0; j < pos; ++j){
				write(fileno(stdout), " ", 1);
			}
			write(fileno(stdout), "\r", 1);
			pos = 0;
			space_cnt = 0;
		}
		if (ch == CTRL_W) {
		    
		    while (pos > 0 && buf_str[pos - 1] == ' ') {
				write(fileno(stdout), "\b \b", 3);
				buf_str[pos--] = '\0';
				space_cnt--;
		    }


		   
		    while (pos > 0 && buf_str[pos - 1] != ' ') {
			    write(fileno(stdout), "\b \b", 3); 
				buf_str[--pos] = '\0';
			}
			printf("%d", space_cnt);
		}

		if (pos == S_LENGTH+1){
			//if (ch == '\n')
				//OK
			//if (buf_str[pos-1] == ' ')
				//OK
			if (buf_str[pos-1] != ' ' && ch != '\n' && space_cnt != 0){
				while(buf_str[--pos] != ' ' && pos != 0){
					write (fileno(stdout), "\b \b", 3);
				}
			}
			
			if(space_cnt == 0){
				write(fileno(stdout), "\n", 1);
				pos = 0;
			}
			
			else if (space_cnt != 0){

				printf("\n\nBUF_STRING:\n");
				for (int j = 0; j < S_LENGTH+1; j++){
					putchar(buf_str[j]);
				}
				printf("\n\nEND\n");

				for (int j = pos; j < S_LENGTH+1; j++)
					write(fileno(stdout), " ", 1);
				write(fileno(stdout), "\n", 1);
				for (int j = pos+1; j < S_LENGTH+1; j++){
					char temp[1] = {buf_str[j]};
					write(fileno(stdout), temp, 1);
				}
				pos = S_LENGTH - pos;
				
			}
		
			space_cnt = 0;
		}
		else{
			putchar(ch);
		}
		
	}
	tcsetattr(fd, TCSAFLUSH, &savtty);
	return 0;
}
