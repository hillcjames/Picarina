#ifndef MAIN
#define MAIN

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>

#include "tones.h"

void inputLoop(int * soundPipe);

void playNote(int * soundPipe, int pitchEnum);

void playFromPipe(int * soundPipe);

enum buttons {
	garbage1,
	garbage2, //so the below match the pin numbers in wiringPi
	b1, //to GPIO2
	b2, //to GPIO3
	b3, //to GPIO4
	b4, //to GPIO5
	O,  //to GPIO6				raise octave
	R,  //to GPIO7				raise a fifth (register, sorta)
	r,  //to GPIO8				lower a fifth
	o   //to GPIO9				lower an octave
};

#endif
