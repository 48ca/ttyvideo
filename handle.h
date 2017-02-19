#ifndef HANDLE_H

#define HANDLE_H

char* filename;

int error(char* message);
int handle(int argc, char** argv);

int terminate;

void sig_handler(int signo);

#endif // HANDLE_H
