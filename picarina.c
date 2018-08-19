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

		// buildPitchList();
		createToneList();

		playNote(soundPipe, A4);
		playNote(soundPipe, B4);
		playNote(soundPipe, C5);

		playNote(soundPipe, A4);
		playNote(soundPipe, B4);
		playNote(soundPipe, C5);

		playNote(soundPipe, A4);
		playNote(soundPipe, B4);
		playNote(soundPipe, C5);

		
		printf("Child done.");
		cleanUp();




		//any input data must be shorter than this
		// long dataSize = 4096 * 64;
		// char * soundData = (char *)calloc(sizeof(char), dataSize);
		//
		// printf("opening\n");
		//
		// int f = open("./tones/F4.wav", O_RDONLY);
		//
		// int * garbage = calloc(1, 4096);
	    // int sz1 = read(f, garbage, 44);
		// printf("%s\n", (char*)garbage);
		// free(garbage);
		// printf("%d\n", sz1);
		//
		// int sz = read(f, soundData, dataSize);
		//
		//
		// //Oh it's so beautiful
		// // I love it
		// int chunkSize = 512;
		// int i, i2;
		// // extend or shorten this loop in order to get rid of skips while
		// //minimizing extra note-time beyond button release.
		// for (i = 0; i < 10; i++ ) {
		//
		// 	int dataRemaining = sz;
		// 	for (i2 = 0; i2 + chunkSize < sz; i2+=chunkSize ) {
		// 		write(soundPipe[1], &(soundData[i2]), chunkSize);
		// 		dataRemaining -= chunkSize;
		// 	}
		// 	write(soundPipe[1], &(soundData[i2]), dataRemaining);
		//
		// }
		// printf("Child done.");
		// free(soundData);
		exit(0);
	}
	// now parent
	close(soundPipe[1]);
	// sleep(2);

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

	return 0;
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
		write(soundPipe[1], &(tone->data[i2]), dataRemaining-2);

	}
}

//
