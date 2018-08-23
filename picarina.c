/*
 * Simple sound playback using ALSA API and libasound.
 *
 * Compile:
 * $ cc -o play sound_playback.c -lasound
 *
 * Usage:
 * $ ./play <sample_rate> <channels> <seconds> < <file>
 *
 * Examples:
 * $ ./play 44100 2 5 < /dev/urandom
 * $ ./play 22050 1 8 < /path/to/file.wav
 *
 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 */

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
// #include <wiringPi.h>

#include "picarina.h"
#include "tones.h"

#define PCM_DEVICE "default"


int main(int argc, char **argv) {

	//build tones
	int soundPipe[2];
	// pipe[0]; //-> Parent reads here
	// pipe[1]; //-> child writes here
	if (pipe(soundPipe) ==-1) {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
	int pid = fork();
	if (pid < 0) { //failure
        fprintf(stderr, "fork Failed" );
        return 1;
	}
	else if (pid == 0) { //child
		//get input continuously, until told to stop.
		//Based on input, pipe a *short* amount of each file to the parent
		//Will probably have to guess and check
		/**
		too long and you'll get a backup in the buffer, too short and I bet
		there'll be gaps.
		*/
		close(soundPipe[0]);

		createToneList();

		inputLoop(soundPipe);



		printf("Child done.");
		cleanUp();

		exit(0);
	}
	// now parent
	close(soundPipe[1]);
	// sleep(2);

	playFromPipe(soundPipe);

	return 0;
}

int buttonMask(int buttonNumber) {
	return 1<<((buttonNumber - 2) * 4);
}

void inputLoop(int * soundPipe) {
	// wiringPiSetup ();
	// pinMode (b1, INPUT);
	// pinMode (b2, INPUT);
	// pinMode (b3, INPUT);
	// pinMode (b4, INPUT);
	//
	// pullUpDnControl (b1, PUD_UP);
	// pullUpDnControl (b2, PUD_UP);
	// pullUpDnControl (b3, PUD_UP);
	// pullUpDnControl (b4, PUD_UP);

	int tonalButtons = 0;
	int registerShift = 0;
	bool inputGood = true;

	int i;
	int note;

	while(inputGood) {
	// for (i = 0; i < 200; i++) {
		tonalButtons = 0;
		registerShift = 0;
		//this could probably be shortened by matching each digitalRead hard-coded
		//value with a value in the buttons enum, so you could just iterate through them
		//all. But this is clearer, and I'm betting this won't be a bottleneck.
		// printf("%d\n", digitalRead(0));

		//Well, as would be expected if I had thought through it, the loop executes
		//faster than the note takes to play. Find some way to check if a note
		//is already being played, and if so, don't send any new data.
		//Shared memory is a thing apparently, and so a simple boolean would work.

		//Note that this leads to something like tonalButtons == 0x1101 or 0x0010,
		//where the rightmost byte/bit (only one bit is used) is b1, and the leftmost
		//is b4.
		// if (digitalRead(b1) == 0) { //lowest bit
		// if (true) {
		// 	tonalButtons |= buttonMask(b1);
		// 	printf("_button 1!\n");
		// 	// playNote(soundPipe, A4);
		// }
		// // if (digitalRead(b2) == 0) {
		// if (false) {
		// 	tonalButtons |= buttonMask(b2);
		// 	printf("_button 2!\n");
		// 	// playNote(soundPipe, B4);
		// }
		// // if (digitalRead(b3) == 0) {
		// if (true) {
		// 	tonalButtons |= buttonMask(b3);
		// 	printf("_button 3!\n");
		// 	// playNote(soundPipe, C5);
		// }
		// // if (digitalRead(b4) == 0) { //highest bit
		// if (true) {
		// 	tonalButtons |= buttonMask(b4);
		// 	printf("_button 4!\n");
		// 	// playNote(soundPipe, D5);
		// }

		for (i = b1; i <= b4; i++) {
			// if (digitalRead(i) == 0) {
			if (i % 3 != 1) {
				tonalButtons |= buttonMask(i);
				printf("button b%d\n", i-1);
			}
		}

		//the shift, in half-steps or array indices, due to the register/octave
		//buttons.
		int t = 1;
		if (t % 2 == 0) {
		// if (digitalRead(O) == 0) {
			registerShift += 12;
		}
		if (t % 3 == 0) {
		// if (digitalRead(R) == 0) {
			registerShift += 7;
		}
		if (t % 5 == 0) {
		// if (digitalRead(r) == 0) {
			registerShift -= 5;
		}
		if (t % 7 == 0) {
		// if (digitalRead(o) == 0) {
			registerShift -= 12;
		}

		if (tonalButtons%2 != 0) {
			inputGood = false;
		}
		note = getNote(tonalButtons, registerShift);
		// printf("___%d  %s\n", note, pitchString(note));
		// printf("___%d  %d  | %s\n",  note, G4,  pitchString(note));
		playNote(soundPipe, note);
		inputGood = false;
	}
	return;
}

int getNote(int tonalButtons, int registerShift) {
	//this returns the index of the note to be played,
	//which comes out to be the number of half-steps above the lowest note.
	//So start at the index for fully open, and decrease it first as the tonal
	//buttons are pressed, and then decrease or increase that value by 8/12 for
	//the registers/octave buttons, respectively.
	//middleC
	int note = G4 + registerShift;
	printf("%x  |  %d   %s\n",  tonalButtons,  registerShift,  pitchString(note));

	switch (tonalButtons) {
		case 0x0000: // open - 			G if no registers pressed
			break;
		case 0x0010: // b2 - 			F# if no registers pressed
			note -= 1;
			break;
		case 0x0001: // b1 - 			F if no registers pressed
			note -= 2;
			break;
		case 0x0011: // b1, b2 - 		E if no registers pressed
			note -= 3;
			break;
		case 0x0101: // b1, b2, b4 - 	Eb if no registers pressed
			note -= 4;
			break;
		case 0x0111: // b1, b2, b3 - 	D if no registers pressed
			note -= 5;
			break;
		case 0x1011: // all - 			C# if no registers pressed
			note -= 6;
			break;
		case 0x1111: // all - 			C if no registers pressed
			note -= 7;
			break;
	}

	printf("%x  |  %d   %s\n",  tonalButtons,  registerShift,  pitchString(note));
	return note;
}

void playNote(int * soundPipe, int pitchEnum) {
	Tone * tone = &tones->tones[pitchEnum];

	int chunkSize = 512;
	int i, i2;
	// extend or shorten this loop in order to get rid of skips while
	//minimizing extra note-time beyond button release.
	for (i = 0; i < 1; i++ ) {

		int dataRemaining = tone->sz;
		for (i2 = 0; i2 + chunkSize <= tone->sz; i2+=chunkSize ) {
			write(soundPipe[1], &(tone->data[i2]), chunkSize);
			dataRemaining -= chunkSize;
		}
		write(soundPipe[1], &(tone->data[i2]), dataRemaining - 2);

	}
}

void playFromPipe(int * soundPipe) {
	unsigned int pcm, tmp, dir;
	int rate, channels, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char * buff;
	int buff_size, loops;

	rate 	 = 44100;
	channels = 1;
	seconds  = 0.1;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0) < 0)
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						SND_PCM_FORMAT_S16_LE) < 0)
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0)
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0)
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	printf("seconds: %d\n", seconds);

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);
	//printf("%d\n", buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

	/**
	loop continuously, reading from pipe whenever it has something.
	Stop when the child stops. sighandler or whatever.
	*/
	while ((pcm = read(soundPipe[0], buff, buff_size)) > 0) {
		printf("..%" PRIu8 "\n", pcm);

		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			fprintf(stderr, "ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
		}
	}

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);
}

//
