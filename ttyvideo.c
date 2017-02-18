#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <stdlib.h>
#include <stdio.h>
#include "handle.h"

int main(int argc, char** argv) {

	int argError;
	argError = handle(argc, argv);
	if(argError) return argError;

	CvCapture* cap = cvCaptureFromFile(filename);

}
