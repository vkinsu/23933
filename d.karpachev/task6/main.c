#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

typedef struct Table_t {
  char* line;
  int offset;
  int length;
} Table;

void Alarm(int var) {
  exit(0);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        perror("Wrong arguments count!\n");
        return 1;
    }

    char* name = argv[1];
    int file = open(name, O_RDONLY);
    if (file == -1) {
        perror("Error with opening file.\n");
        return 1;
    }

    Table* table = NULL;

    char buf[2];
    int pos = 0;
    int line_count = 1;
    table = (Table*)malloc(sizeof(Table));
    while (read(file, buf, 1) > 0) {
        if (buf[0] == '\n') {
          table = (Table*)realloc(table, (++line_count) * sizeof(Table));
          table[line_count - 1].line = NULL;
          table[line_count - 1].offset = pos + 1;
          table[line_count - 1].length = 0;
        }
        else {
          table[line_count - 1].length++;
        }
        pos++;
    }
    line_count--;

    for (int i = 0; i < line_count; i++) {
      if (table[i].length > 0) {
        table[i].line = (char*)malloc(table[i].length + 1);
        lseek(file, table[i].offset, 0);
        read(file, table[i].line, table[i].length);
        table[i].line[table[i].length] = '\0';
      }
    }

    char num[1024];
    int idx = 0;
    signal(SIGALRM, Alarm);
    bool flag = true;
    alarm(5);
    while(scanf("%s", num)) {
      if (flag) {
        alarm(0);
        flag = false;
      }
      idx = atoi(num);
      if (idx == 0)
        break;
      if (idx > line_count || idx < 0) {
        perror("Wrong line number!\n");
        continue;
      }
      if (table[idx - 1].length > 0)
        printf("%s\n", table[idx - 1].line);
      else
        printf("\n");
    }

    for (int i = 0; i < line_count; i++) {
        if (table[i].line != NULL)
          free(table[i].line);
    }
    free(table);

    close(file);

    return 0;
}
