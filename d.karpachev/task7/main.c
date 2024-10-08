#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>

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

    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error with opening file.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long int file_size = ftell(file);
    rewind(file);

    char *file_data = NULL;
    file_data = (char*)malloc(file_size + 1);
    size_t bytes_read = fread(file_data, 1, file_size, file);
    file_data[file_size] = '\0';
    fclose(file);

    Table* table = NULL;

    int pos = 0;
    int line_count = 0;
    for (int c = 0; c < file_size; c++) {
        if (file_data[c] == '\n') {
          if (table == NULL) {
            table = (Table*)malloc((line_count + 1) * sizeof(Table));
          }
          else {
            table = (Table*)realloc(table, (line_count + 1) * sizeof(Table));
          }
          line_count++;
          table[line_count - 1].offset = pos + 1;
          table[line_count - 1].line = NULL;
          table[line_count - 1].length = 0;
        }
        else {
          if (table == NULL) {
            table = (Table*)malloc((line_count + 1) * sizeof(Table));
            line_count++;
            table[line_count - 1].offset = pos;
            table[line_count - 1].line = NULL;
            table[line_count - 1].length = 0;
          }
          table[line_count - 1].length++;
        }
        pos++;
    }
    line_count--;

    for (int i = 0; i < line_count; i++) {
      if (table[i].length > 0) {
        table[i].line = (char*)malloc(table[i].length + 1);
        for (int j = 0; j < table[i].length; j++) {
          table[i].line[j] = file_data[table[i].offset + j];
        }
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

    free(table);
    free(file_data);

    return 0;
}
