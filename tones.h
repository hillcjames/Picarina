#ifndef TONE
#define TONE

typedef struct ToneList {
	struct Tone * tones;
	int sz;
} ToneList;

typedef struct Tone {
	char * data;
	int sz;
} Tone;

typedef struct Pitch {
	char * n;
} Pitch;

void initTone(Tone * t, char * path);

void createToneList();

// void buildPitchList();

void cleanUp();

void deallocTone(Tone * t);
void deallocToneList(ToneList * tones );

Pitch * pitches;
int numPitches;
ToneList * tones;


char * pitchString(int pitchEnum);

enum pitchIndx {
	G2, 	//  0
	Ab2, 	//  1
	A2, 	//
	Bb2, 	//
	B2, 	//
	C3, 	//
	Cs3, 	//
	D3, 	//
	Eb3, 	//
	E3, 	//
	F3, 	//
	Fs3, 	//
	G3, 	//
	Ab3, 	//
	A3, 	//
	Bb3, 	//
	B3, 	//
	C4, 	//
	Cs4, 	//
	D4, 	//
	Eb4, 	//
	E4, 	//
	F4, 	//
	Fs4, 	//  2
	G4, 	//  2
	Ab4, 	//  2
	A4, 	//  2
	Bb4, 	//  2
	B4, 	//  2
	C5, 	//  2
	Cs5, 	//  2
	D5, 	//  2
	Eb5, 	//  2
	E5, 	//  3
	F5, 	//  3
	Fs5, 	//  3
	G5, 	//  3
	Ab5, 	//  3
	A5, 	//  3
	Bb5, 	//  3
	B5, 	//  3
	C6 		//  3
};

#endif
