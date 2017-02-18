#ifndef HANDLE_H

#define HANDLE_H

char* filename;
char* outfile;

int error(char* message);
int handle(int argc, char** argv);

#endif // HANDLE_H
