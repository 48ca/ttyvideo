#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/imgproc_c.h>
// #include <opencv2/highgui/highgui_c.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "handle.h"
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define NANO_CONV_FACTOR 1000000000

#define COLOR_FORMAT "\e[38;05;%umA"

int waitFrame(uint64_t, uint64_t);
unsigned char generateANSIColor(unsigned char, unsigned char, unsigned char);

int main(int argc, char** argv) {

	int argError;
	argError = handle(argc, argv);
	if(argError) return argError;

	CvCapture* cap = cvCaptureFromFile(filename);
	if(!cap) return error("Can't read input");

	//

	int tty_width, tty_height;

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	tty_width = w.ws_col;
	tty_height = w.ws_row;

	//

	register int width, height, nchannels, step, offset;
	register int i, j;
	register unsigned char r_ch, b_ch, g_ch;

	double fps = (double)cvGetCaptureProperty(cap, CV_CAP_PROP_FPS);

	register int frameNum = 0;

	IplImage* frame;

	struct timespec start, end;

	uint64_t delta_ns;
	uint64_t delayNecessary = NANO_CONV_FACTOR/fps;

	int ansiColor;

	for(;;) {

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		frame = cvQueryFrame(cap);

		if(!frame) break;

		system("clear");

		width = frame->width;
		height = frame->height;
		nchannels = frame->nChannels;
		step = frame->widthStep;

		for(i = 0; i < tty_height; ++i) {
			for(j = 0; j < tty_width; ++j) {
				unsigned char* data = (unsigned char*)(frame->imageData + i*step*height/tty_height);
				offset = j * nchannels * width / tty_width;
				b_ch = data[offset];
				g_ch = data[offset+1];
				r_ch = data[offset+2];

				ansiColor = generateANSIColor(r_ch, g_ch, b_ch);

				// printf("%u %u %u :: %u ", r_ch, g_ch, b_ch, ansiColor);
				printf(COLOR_FORMAT, ansiColor);
			}
			printf("\n");
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		delta_ns = (end.tv_sec - start.tv_sec) * NANO_CONV_FACTOR + (end.tv_nsec - start.tv_nsec);

		waitFrame(delayNecessary * 100, delta_ns);

	}

	cvReleaseImage(&frame);

	return 0;

}

int waitFrame(uint64_t delayNecessary, uint64_t delta_ns) {

	struct timespec ts;
	long totalNanoSec;

	totalNanoSec = (delayNecessary - delta_ns);

	if(totalNanoSec < 0) return 1;

	ts.tv_sec = totalNanoSec/NANO_CONV_FACTOR;
	ts.tv_nsec = totalNanoSec%NANO_CONV_FACTOR;
	nanosleep(&ts, NULL);

	return 0;

}

unsigned char generateANSIColor(unsigned char r, unsigned char g, unsigned char b) {
	return 16 + 36 * r*6/256 + 6 * g*6/256 + b*6/256;
}
