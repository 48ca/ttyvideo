#ifndef HANDLE_H
#define HANDLE_H

#ifdef __cplusplus
extern "C" {
#endif

char* filename;

int error(char* message);
int handle(int argc, char** argv);

int terminate;

void sig_handler(int signo);

#ifdef __cplusplus
}
#endif

#endif // HANDLE_H
