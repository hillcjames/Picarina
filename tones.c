#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tones.h"

int maxSize = 4096*64;

ToneList * toneList;


void initTone(Tone * t, char * path) {
	t->data = calloc(sizeof(char), maxSize);

	int f = open(path, O_RDONLY);

	int * garbage = calloc(1, 64);
	int sz1 = read(f, garbage, 44);
	free(garbage);

	t->sz = read(f, t->data, maxSize);
}



void createToneList() {
	numPitches = 40;
	tones = calloc(sizeof(ToneList), 1);
	tones->sz = numPitches;
	tones->tones = calloc(sizeof(Tone), tones->sz);

	char * str = calloc(sizeof(char), 16);
	int i;
	for (i = 0; i < numPitches; i++ ) {
		sprintf(str, "./tones/%s.wav", pitchString(i));
		printf("loading tone: \'%s\'\n", str);
		initTone(&(tones->tones[i]), str);
	}
	free(str);
}

void cleanUp() {
	free(pitches);
	deallocToneList(tones);
}

void deallocTone(Tone * t) {
	free(t->data);
	free(t);
}

void deallocToneList(ToneList * tones ) {
	int i=0;
	for(i = 0; i < tones->sz; i++) {
		deallocTone(&(tones->tones[i]));
	}
	free(tones);
}


char * pitchString(int pitchEnum) {
	switch (pitchEnum) {
		case G2:
			return "G2";
		case Ab2:
			return "Ab2";
		case A2:
			return "A2";
		case Bb2:
			return "Bb2";
		case B2:
			return "B2";
		case C3:
			return "C3";
		case Cs3:
			return "Cs3";
		case D3:
			return "D3";
		case Eb3:
			return "Eb3";
		case E3:
			return "E3";
		case F3:
			return "F3";
		case Fs3:
			return "F#3";
		case G3:
			return "G3";
		case Ab3:
			return "Ab3";
		case A3:
			return "A3";
		case Bb3:
			return "Bb3";
		case B3:
			return "B3";
		case C4:
			return "C4";
		case Cs4:
			return "C#4";
		case D4:
			return "D4";
		case Eb4:
			return "Eb4";
		case E4:
			return "E4";
		case F4:
			return "F4";
		case Fs4:
			return "F#4";
		case G4:
			return "G4";
		case Ab4:
			return "Ab4";
		case A4:
			return "A4";
		case Bb4:
			return "Bb4";
		case B4:
			return "B4";
		case C5:
			return "C5";
		case Cs5:
			return "C#5";
		case D5:
			return "D5";
		case Eb5:
			return "Eb5";
		case E5:
			return "E5";
		case F5:
			return "F5";
		case Fs5:
			return "F#5";
		case G5:
			return "G5";
		case Ab5:
			return "Ab5";
		case A5:
			return "A5";
		case Bb5:
			return "Bb5";
		case B5:
			return "B5";
		case C6:
			return "C6";
	}
}


//
// void buildPitchList() {
// 	numPitches = 36
// 	pitches = calloc(sizeof(Pitch), numPitches);
// 	int i;
// 	for (i = 0; i < numPitches; i++ ) {
// 		pitches[i].n = pitchString(i);
// 	}
// 		// pitches[G2].n = "G2";
// 		// pitches[Ab2].n = "Ab2";
// 		// pitches[B2].n = "B2";
// 		// pitches[Bb2].n = "Bb2";
// 		// pitches[C3].n = "C3";
// 		// pitches[Cs3].n = "Cs3";
// 		// pitches[D3].n = "D3";
// 		// pitches[Eb3].n = "Eb3";
// 		// pitches[E3].n = "E3";
// 		// pitches[F3].n = "F3";
// 		// pitches[Fs3].n = "F#3";
// 		// pitches[G3].n = "G3";
// 		// pitches[Ab3].n = "Ab3";
// 		// pitches[A3].n = "A3";
// 		// pitches[Bb3].n = "Bb3";
// 		// pitches[C4].n = "C4";
// 		// pitches[Cs4].n = "C#4";
// 		// pitches[D4].n = "D4";
// 		// pitches[Eb4].n = "Eb4";
// 		// pitches[E4].n = "E4";
// 		// pitches[F4].n = "F4";
// 		// pitches[Fs4].n = "F#4";
// 		// pitches[G4].n = "G4";
// 		// pitches[Ab4].n = "Ab4";
// 		// pitches[A4].n = "A4";
// 		// pitches[Bb4].n = "Bb4";
// 		// pitches[C5].n = "C5";
// 		// pitches[Cs5].n = "C#5";
// 		// pitches[D5].n = "D5";
// 		// pitches[Eb5].n = "Eb5";
// 		// pitches[E5].n = "E5";
// 		// pitches[F5].n = "F5";
// 		// pitches[Fs5].n = "F#5";
// 		// pitches[G5].n = "G5";
// 		// pitches[Ab5].n = "Ab5";
// 		// pitches[A5].n = "A5";
// 		// pitches[Bb5].n = "Bb5";
// 		// pitches[B5].n = "B5";
// 		// pitches[C6].n = "C6";
// }

//
