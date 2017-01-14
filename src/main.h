#ifndef _MAIN_H_
#define _MAIN_H_

#include "common/types.h"
#include "dynamic_libs/os_functions.h"

/* Main */
#ifdef __cplusplus
extern "C" {
#endif

//! C wrapper for our C++ functions
int Menu_Main(void);

#ifdef __cplusplus
}
#endif

typedef unsigned char sample;

typedef struct {
	void* voice;

	sample* samples;
	int numSamples;

	int modified; //data has been modified and is ready to be played
	int stopRequested; //request to stop the voice
	int stopped; //whether the voice has stopped
	int newSoundRequested; //request to change the tone of the voice
	float newFrequency; //frequency of new tone
	int nextDataRequested; //request the audio callback prepare for a new set of samples
	int readyForNextData; //audio callback has prepared for new samples
} voiceData;

typedef struct _ax_buffer_t {
    u16 format;
    u16 loop;
    u32 loop_offset;
    u32 end_pos;
    u32 cur_pos;
    const unsigned char *samples;
} ax_buffer_t;

#define SAMPLE_BUFFER_MAX_SIZE 0x2000 //size of your average sample buffer

#endif
