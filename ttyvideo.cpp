// #include <opencv2/core/core_c.h>
// #include <opencv2/videoio/videoio_c.h>
// #include <opencv2/videoio/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "handle.h"
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define NANO_CONV_FACTOR 1000000000

#define COLOR_FORMAT "\x1B[48;05;%um "

int waitFrame(uint64_t, uint64_t);
unsigned char generateANSIColor(unsigned char, unsigned char, unsigned char);
void getTTYDims();

int tty_width;
int tty_height;

int main(int argc, char** argv) {

	int argError;
	argError = handle(argc, argv);
	if(argError) return argError;

	cv::VideoCapture cap(filename);
	if(!cap.isOpened()) return error((char*)"Can't read input");

	getTTYDims();

	register int width, height, nchannels, step, offset;
	register int i, j;
	register unsigned char r_ch, b_ch, g_ch;

	double fps = cap.get(CV_CAP_PROP_FPS);

	register int frameNum = 0;

	cv::Mat frame;

	struct timespec start, end;

	uint64_t delta_ns;
	uint64_t delayNecessary = fps == 0 || isnan(fps) ? 0 : NANO_CONV_FACTOR/fps;

	int ansiColor;

	unsigned char* data;

	setvbuf(stdout, NULL, _IOFBF, 0);

	signal(SIGINT, sig_handler);
	terminate = 0;

	for(;;) {

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		if(!frame.empty()) {

			cap >> frame;

			if(frame.empty()) break;

			for(i = 0; i < tty_height - 1; ++i)
				printf("\x1B[F");

		} else {

			cap >> frame;

			if(frame.empty()) break;

		}

		if(frame.depth() != CV_8U) {
			return error((char*)"Frame has incorrect depth");
		}

		width = frame.cols;
		height = frame.rows;
		nchannels = frame.channels();
		step = width * nchannels;

		getTTYDims();

		for(i = 0; i < tty_height; ++i) {
			data = (unsigned char*)(frame.data + ((int)((float)i*height/tty_height)*step));
			for(j = 0; j < tty_width; ++j) {
				offset = (int)((float)j*width/tty_width) * nchannels;
				b_ch = data[offset];
				g_ch = data[offset+1];
				r_ch = data[offset+2];

				ansiColor = generateANSIColor(r_ch, g_ch, b_ch);

				printf(COLOR_FORMAT, ansiColor);
			}
			if(i < tty_height - 1) {
				printf("\n");
			} else {
				printf("\x1B[m");
				fflush(stdout);
			}
		}

		if(terminate) {
			// Wait for 10 milliseconds to let stdout finish its business
			waitFrame(10000, 0);
			break;
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		delta_ns = (end.tv_sec - start.tv_sec) * NANO_CONV_FACTOR + (end.tv_nsec - start.tv_nsec);

		waitFrame(delayNecessary, delta_ns);

		frameNum++;

	}

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

	return 16 + (36 * lround(r*5.0/256)) + (6 * lround(g*5.0/256)) + lround(b*5.0/256);

}

void getTTYDims() {

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	tty_width = w.ws_col;
	tty_height = w.ws_row;

}
