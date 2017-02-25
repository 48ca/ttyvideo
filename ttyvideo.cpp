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
int play(char*, char*, int);

int tty_width;
int tty_height;
int tty_width_custom = 0;
int tty_height_custom = 0;

int no_interrupts = 0;

#define C (char*)

int main(int argc, char** argv) {

	char* filename       = setDefaultArgument(C"infile");
	char* width_option   = addArgument(C"Output width", TAKES_ONE_ARGUMENT, C"-w", C"--width");
	char* height_option  = addArgument(C"Output height", TAKES_ONE_ARGUMENT, C"-h", C"--height");
	char* sleep_option   = addArgument(C"Add a pause between loops or after plays (seconds)", TAKES_ONE_ARGUMENT, C"-s", C"--sleep");
	char* pause_option   = addArgument(C"Add a pause between loops or after plays (ms)", TAKES_ONE_ARGUMENT, C"-p", C"--pause");
	char* noexit_option  = addArgument(C"Prevent the program for exiting", TAKES_NO_ARGUMENTS, C"--no-exit", NULL);
	char* loop_option    = addArgument(C"Loop videos", TAKES_NO_ARGUMENTS, C"-l", C"--loop");
	char* help_option    = addArgument(C"Print usage", TAKES_NO_ARGUMENTS, C"--help", NULL);
	char* nointer_option = addArgument(C"No interrupts", TAKES_NO_ARGUMENTS, C"--no-interrupts", NULL);
	char* fps_option     = addArgument(C"FPS", TAKES_ONE_ARGUMENT, C"--fps", NULL);

	int argError;
	argError = handle(argc, argv);
	if(argError) {
		printUsage();
		return argError;
	}

	if(help_option[0] != '\0') {
		printUsage();
		return 0;
	}

	if(filename[0] == '\0')
		return error((char*)"No input file specified");

	getTTYDims(); // get initial dims

	tty_height_custom = atoi(height_option);
	tty_width_custom = atoi(width_option);

	if(tty_height_custom > tty_height || tty_width_custom > tty_width) {
		error((char*)"The specified dimensions are too large, but we will play the video anyway");
		sleep(1);
	}

	getTTYDims(); // get real dims -- including user specification

	int sleep_time = sleep_option[0] == '\0' ? 0 : atoi(sleep_option);
	int pause_time = pause_option[0] == '\0' ? 0 : atoi(pause_option);

	int noexit = noexit_option[0] == '\0' ? 0 : 1;

	int loop = loop_option[0] == '\0' ? 0 : 1;

	setvbuf(stdout, NULL, _IOFBF, 0);

	signal(SIGINT, sig_handler);

	no_interrupts = nointer_option[0] == '\0' ? 0 : 1;

	register int frameNum = 0;
	do {
		frameNum = play(filename, fps_option, frameNum);

		if(terminate && !no_interrupts) {
			printf("\n");
			fflush(stdout);
			return 0;
		}

		if(pause_time)
			waitFrame(1000 * pause_time, 0);
		if(sleep_time)
			sleep(sleep_time);

	} while(loop && frameNum > 1);

	if(noexit)
		while(1)
			sleep(60);

	return 0;

}

int play(char* filename, char* fps_option, int subsequentPlay) {
	cv::VideoCapture cap(filename);
	if(!cap.isOpened()) return error((char*)"Can't read input");

	double fps = fps_option[0] == '\0' ? cap.get(CV_CAP_PROP_FPS) : atof(fps_option);

	struct timespec start, end;

	uint64_t delta_ns;
	uint64_t delayNecessary = fps == 0 || isnan(fps) ? 0 : NANO_CONV_FACTOR/fps;

	register int frameNum = 0;

	register int width, height, nchannels, step, offset;
	register int i, j;
	register unsigned char r_ch, b_ch, g_ch;
	cv::Mat frame;
	int ansiColor;
	unsigned char* data;

	for(;;) {

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		if(!frame.empty() || subsequentPlay) {

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

		if(terminate && !no_interrupts) {
			// Wait for 10 milliseconds to let stdout finish its business
			waitFrame(10000, 0);
			break;
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		delta_ns = (end.tv_sec - start.tv_sec) * NANO_CONV_FACTOR + (end.tv_nsec - start.tv_nsec);

		waitFrame(delayNecessary, delta_ns);

		frameNum++;

	}

	return frameNum;

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
	tty_width = tty_width_custom > 0 ? tty_width_custom : w.ws_col;
	tty_height = tty_height_custom > 0 ? tty_height_custom : w.ws_row;

}
