#include <opencv2/core/core_c.h>
// #include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
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

	register int width, height, nchannels, step, offset;
	register int i, j, r_ch, b_ch, g_ch;

	double fps = (double)cvGetCaptureProperty(cap, CV_CAP_PROP_FPS);

	register int frameNum = 0;

	IplImage* frame;

   	frame = cvQueryFrame(cap);

	cvNamedWindow("w", 1);

	while(frame != NULL) {

		width = frame->width;
		height = frame->height;
		nchannels = frame->nChannels;
		step = frame->widthStep;

		for(i = 0; i < height; ++i) {
			for(j = 0; j < width; ++j) {
				unsigned char* data = (unsigned char*)(frame->imageData + i*step);
				offset = j * nchannels;
				b_ch = data[offset];
				g_ch = data[offset+1];
				r_ch = data[offset+2];
			}
		}

		printf("Finished parsing frame %d\n", frameNum++);
		printf("Final bits: %d %d %d\n", b_ch, g_ch, r_ch);

		cvShowImage("w", frame);

		cvWaitKey(1000/fps);

		frame = cvQueryFrame(cap);

	}

	cvDestroyWindow("w");
	cvReleaseImage(&frame);

	return 0;

}
