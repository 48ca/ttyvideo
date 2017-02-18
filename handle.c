#include "handle.h"
#include <stdio.h>

int error(char* message) {

	printf("%s: %s\n", "ttyvideo", message);

	return 1;

}

int handle(int argc, char** argv) {

	if(argc >= 3) {
		printf("usage: ttyvideo <infile>\n");
		return 2;
	}

	switch(argc) {
		case(2):
			filename = argv[1];
			break;
		case(1):
			return error("No input file specified");
	}

	return 0;

}
