#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "handle.h"

int main(int argc, char** argv) {

	int argError;
	argError = handle(argc, argv);
	if(argError) return argError;

	CvCapture* cap = cvCaptureFromFile(filename);
	if(!cap) return error("Can't read input");

	IplImage* frame = cvQueryFrame(cap);

	double fps = (double)cvGetCaptureProperty(cap, CV_CAP_PROP_FPS);

	printf("%f\n", fps);

}
