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

//import array of tone data - short.
//

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

		// sleep(1);
		// char msg[] = { "Can you hear me now?\n" };
//        char msg[] = { "Hi\n" };
		// close(soundPipe[0]);
        // write(soundPipe[1], msg, sizeof msg);
        // char cdone[] = "Child now exiting ...\n";
        // write(1, cdone, sizeof cdone);

		close(soundPipe[0]);

		// char * garbage = calloc(8, 128);
	    // read(0, garbage, 44);
		// // printf("%s\n", garbage);
		// free(garbage);

		// char * soundBuffer = (char *)calloc(sizeof(char), 4096*256);

		//any input data must be shorter than this
		long dataSize = 4096 * 64;
		char * soundData = (char *)calloc(sizeof(char), dataSize);
		// int f = open("./txt", O_RDONLY);
		printf("opening\n");
		int f = open("./tones/F4.wav", O_RDONLY);
		printf("done opening\n");

		//might try using an actual byte datatype - it looks like read 260
		//reads 2340, and I need it to read 2348: chunks of 8 not 9.. why on earth
		//is it chunks of 9?
		//and that's only to get an even multiple of 4096 - it's probably not
		//going to work anyway
		//let's assume I need chunks of 4, to get to the mentioned 44 byte mark.

		int * garbage = calloc(1, 4096);
	    int sz1 = read(f, garbage, 44);
		printf("%s\n", (char*)garbage);
		free(garbage);
		printf("%d\n", sz1);

		int sz = read(f, soundData, dataSize);


		//Oh it's so beautiful
		// I love it
		int chunkSize = 1024;
		int i, i2;
		// extend or shorten this loop in order to get rid of skips while
		//minimizing extra note-time beyond button release.
		for (i = 0; i < 10; i++ ) {

			int dataRemaining = sz;
			for (i2 = 0; i2 < sz; i2+=chunkSize ) {
				if ( dataRemaining >= chunkSize ) {
					write(soundPipe[1], &(soundData[i2]), chunkSize);
				}
				else {
					write(soundPipe[1], &(soundData[i2]), dataRemaining);
				}
				dataRemaining -= chunkSize;
			}

			//skips
			// char * iter = soundData;
			// // for (i2 = 0; i2 < sz/chunkSize; i2++) {
			// while(iter < soundData + sz/chunkSize) {
			// 	// int sz = read(f, soundData, 4096);
			// 	_4096bit* tmp = iter;
			// 	printf("%d\n", sizeof(tmp));
			// 	printf("%p %p   %p\n", tmp, iter, iter-tmp);
			// 	write(soundPipe[1], tmp, 256); //works, but wrong pitch??
			// 	iter++;
			// }


			//skips
			// _4096byte* iter = start;
			// _4096byte* tmp;
			// // for (i2 = 0; i2 < sz/chunkSize; i2++) {
			// while(iter < start + sz/chunkSize) {
			// 	// printf("%p\n", iter);
			// 	// int sz = read(f, soundData, 4096);
			//
			// 	// int i3;
			// 	// for (i3 = 0; i3 < 4096; i3++ ) {
			// 	// 	// printf("\t%x\n", iter->buf[i3]);
			// 	// 	// int l = 0;
			// 	// 	// for (l = 0; l < 8; l++) {
			// 	// 		// printf("\t\t");
			// 	// 		// int l2;
			// 	// 		// for (l2 = 0; l2<(iter->buf[i3] & (1<<l)); l2++) {
			// 	// 		// 	printf(" ");
			// 	// 		// }
			// 	// 		// printf("%d \n", iter->buf[i3] & (1<<l));
			// 	// 	// }
			// 	// 	write(soundPipe[1], &(iter->buf[i3]), 1);
			// 	// }
			// 	tmp = iter;
			// 	// printf("%d\n", sizeof(tmp));
			// 	// printf("%p %p   %p\n", tmp, iter, iter-tmp);
			// 	write(soundPipe[1], tmp, 4096); //works, but wrong pitch??
			// 	iter++;
			// }

			//old
			// write(soundPipe[1], (char*)iter, sz%chunkSize);
			// int sz = -1;
			// while(sz != 0) {
			// 	sz = read(f, soundBuffer, 4096*256);
			// 	// printf("%d\n", sz);
			// 	write(soundPipe[1], soundBuffer, sz);
			// 	// printf("..\n");
			// }
		}
		printf("Child done.");
		free(soundData);
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

	// char pipe_buf[buff_size]; //less than the length of the input message
	// char bread, pipe_buf[12]; //less than the length of the input message
	// char csaid[] = "Child says: ";
	// write(1, csaid, sizeof csaid);

	// while ((bread = read(soundPipe[0], pipe_buf, sizeof pipe_buf)) > 0)
	// 	write(1, pipe_buf, bread);
	// exit(0);

	// bool dunno = true;
	// while (dunno) {
	// for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {

	while ((pcm = read(soundPipe[0], buff, buff_size)) > 0) {
		// if (pcm = read(soundPipe[0], buff, buff_size) == 0) {
		// 	fprintf(stderr, "Early end of file.\n");
		// 	return 0;
		// }
		printf("..%" PRIu8 "\n", pcm);

		if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
			// printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			fprintf(stderr, "ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
		}
	}

	//
	// for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {
	// 	if (pcm = read(0, buff, buff_size) == 0) {
	// 		printf("Early end of file.\n");
	// 		return 0;
	// 	}
	//
	// 	if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
	// 		printf("XRUN.\n");
	// 		snd_pcm_prepare(pcm_handle);
	// 	} else if (pcm < 0) {
	// 		printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
	// 	}
	//
	// }

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <alsa/asoundlib.h>
// #include <alsa/pcm.h>
//
// int main() {
//   int val;
//
//   printf("ALSA library version: %s\n", SND_LIB_VERSION_STR);
//
//   printf("\nPCM stream types:\n");
//   for (val = 0; val <= SND_PCM_STREAM_LAST; val++)
//     printf("  %s\n",
//       snd_pcm_stream_name((snd_pcm_stream_t)val));
//
//   return 0;
// }
