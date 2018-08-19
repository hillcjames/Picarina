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

bool setUp();

void playNote(int * soundPipe, int pitchEnum);

#endif
