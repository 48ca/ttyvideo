#include "handle.h"
#include <stdio.h>

int error(char* message) {

	printf("%s: %s\n", "ttyvideo", message);

}

int handle(int argc, char** argv) {

	switch(argc) {
		case(3):
			outfile = argv[2];
		case(2):
			filename = argv[1];
			break;
		case(1):
			return error("No input file specified");
	}

	return 0;

}
