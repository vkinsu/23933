#ifndef SERVER_H
#define SERVER_H

extern int beg_messages[2];
extern int end_messages[2];

int server_run(int argc, char** argv, const char* client_path);

#endif

