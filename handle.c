#include "handle.h"
#include <stdio.h>

int handle(int argc, char** argv) {

	switch(argc) {
		case(3):
			outfile = argv[2];
		case(2):
			filename = argv[1];
			break;
		case(1):
			printf("%s\n", "No input file specified.");
			return 1;
	}

	return 0;

}
