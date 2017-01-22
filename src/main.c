#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/fs_functions.h"
#include "dynamic_libs/gx2_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/padscore_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "dynamic_libs/ax_functions.h"
#include "fs/fs_utils.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "common/common.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "main.h"

//Custom print function
#define PRINTU(x, y, ...) { snprintf(msg, 80, __VA_ARGS__); OSScreenPutFontEx(0, x, y, msg); OSScreenPutFontEx(1, x, y, msg); }

void axFrameCallback();
void playAlarm();

unsigned int generateSineWave(sample* samples, unsigned int maxLength, float freq);

voiceData voice1; //voiceData is in main.h

// Entry point
int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    InitOSFunctionPointers();
    InitSocketFunctionPointers();

    log_init("192.168.1.143");
    log_print("Starting launcher\n");

    InitAXFunctionPointers();
    InitFSFunctionPointers();
    InitVPadFunctionPointers();

    log_print("Function exports loaded\n");

    //!*******************************************************************
    //!                    Initialize heap memory                        *
    //!*******************************************************************
    log_print("Initialize memory management\n");
    //! We don't need bucket and MEM1 memory so no need to initialize
    memoryInitialize();

    //!*******************************************************************
    //!                        Initialize FS                             *
    //!*******************************************************************
    log_printf("Mount SD partition\n");
    mount_sd_fat("sd");

    VPADInit();

    // Prepare screen
    int screen_buf0_size = 0;
    int screen_buf1_size = 0;

    // Init screen and screen buffers
    OSScreenInit();
    screen_buf0_size = OSScreenGetBufferSizeEx(0);
    screen_buf1_size = OSScreenGetBufferSizeEx(1);

    unsigned char *screenBuffer = MEM1_alloc(screen_buf0_size + screen_buf1_size, 0x40);

    OSScreenSetBufferEx(0, screenBuffer);
    OSScreenSetBufferEx(1, (screenBuffer + screen_buf0_size));

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    //Init stuff
    char msg[80];
    bool playing = 0;
    uint8_t sel_digit = 1;
    uint8_t alarmTime[2] = {0, 0};
    OSCalendarTime time;
    int vpadError = -1;
    int delay = 0;
    VPADData vpad;

    //AX stuff

    //init 48KHz renderer
	unsigned int params[3] = {1, 0, 0};
	AXInitWithParams(params);
	AXRegisterFrameCallback((void*)axFrameCallback); //this callback doesn't really do much
    log_print("initialized render \n");

	memset((void*)&voice1, 0, sizeof(voice1));
	voice1.samples = malloc(SAMPLE_BUFFER_MAX_SIZE); //allocate room for samples
    log_print("allocated voice memory");

	voice1.stopped = 1;

	voice1.voice = AXAcquireVoice(25, 0, 0); //get a voice, priority 25. Just a random number I picked
	if (!voice1.voice) {
		return;
	}
	AXVoiceBegin(voice1.voice);
	AXSetVoiceType(voice1.voice, 0);
	log_print("aquired voice \n");

	//Set volume?
	unsigned int vol = 0x80000000;
	AXSetVoiceVe(voice1.voice, &vol);

	//Device mix? Volume of DRC/TV?
	unsigned int mix[24];
	memset(mix, 0, sizeof(mix));
	mix[0] = vol;
	mix[4] = vol;

	AXSetVoiceDeviceMix(voice1.voice, 0, 0, mix);
	AXSetVoiceDeviceMix(voice1.voice, 1, 0, mix);
	log_print("set volume \n");

    AXVoiceEnd(voice1.voice);

    while(1)
    {
        VPADRead(0, &vpad, 1, &vpadError);

        OSTicksToCalendarTime(OSGetTime(), &time);
        int64_t totalSeconds = OSGetTime()/SECS_TO_TICKS(1);
        int currentMinute = (totalSeconds % 3600) / 60;
        int currentHour = (totalSeconds % 86400) / 3600;

        OSScreenClearBufferEx(0, 0);
        OSScreenClearBufferEx(1, 0);

        PRINTU(14, 1, "wiiALARMu 1.0 by opendata")
        PRINTU(0, 14, "An alarm is automatically set when you change the time")

        uint8_t x_shift = 12+3*sel_digit;
        PRINTU(x_shift, 6, "vv");

        PRINTU(0, 7, "Alarm time: %2d:%2d", alarmTime[0], alarmTime[1]);
        PRINTU(0, 12, "Current time: %d:%d", currentHour, currentMinute);


		if (voice1.newSoundRequested)
        {
			//clear the flag
			voice1.newSoundRequested = 0;

			//wipe the existing samples
			memset(voice1.samples, 0, SAMPLE_BUFFER_MAX_SIZE);

            //generate sample
            voice1.numSamples = generateSineWave(voice1.samples, SAMPLE_BUFFER_MAX_SIZE, voice1.newFrequency);

			//flush the samples to memory
			DCFlushRange(voice1.samples, SAMPLE_BUFFER_MAX_SIZE);
			//mark the voice as modified so the audio callback will reload it
			voice1.modified = 1;
			//flush changes to memory
			DCFlushRange(&voice1, sizeof(voice1));
		}


        if (alarmTime[1] == currentMinute && alarmTime[0] == currentHour)
        {
            //TODO: fix this so alarm repeats and isnt a static sound
          /*if (time.msec % 1000 == 0){
                playAlarm();
            }
            if (time.msec % 1500 == 0){
                voice1.stopRequested = 1;
                DCFlushRange(&voice1, sizeof(voice1));

            }*/
            if(!playing){
                playAlarm();
                playing = true;
            }

        } else {
            playing = false;
            voice1.stopRequested = 1;
            DCFlushRange(&voice1, sizeof(voice1));
        }

        OSScreenFlipBuffersEx(0);
        OSScreenFlipBuffersEx(1);

        u32 pressedBtns = vpad.btns_d | vpad.btns_h;

        // Check for buttons
        // Left/Right Buttons
        if (vpad.btns_d & VPAD_BUTTON_LEFT )
        {
            if(sel_digit == 0)
                sel_digit = 1;
            else
                --sel_digit;
        }

        if (vpad.btns_d & VPAD_BUTTON_RIGHT)
        {
            sel_digit = ((sel_digit + 1) % 2);
        }

        // Up/Down Buttons
        if (pressedBtns & VPAD_BUTTON_UP)
        {
            if(--delay <= 0 && sel_digit == 0 && alarmTime[sel_digit] == 23) {
                alarmTime[sel_digit] = 0;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            } else if (--delay <= 0 && sel_digit == 1 && alarmTime[sel_digit] == 59) {
                alarmTime[sel_digit] = 0;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            } else if (--delay <= 0) {
                alarmTime[sel_digit] ++;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            }
        }
        else if (pressedBtns & VPAD_BUTTON_DOWN)
        {
            if(--delay <= 0 && sel_digit == 0 && alarmTime[sel_digit] == 0) {
                alarmTime[sel_digit] = 23;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            } else if (--delay <= 0 && sel_digit == 1 && alarmTime[sel_digit] == 0) {
                alarmTime[sel_digit] = 59;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            } else if (--delay <= 0) {
                alarmTime[sel_digit] --;
                delay = (vpad.btns_d & VPAD_BUTTON_UP) ? 6 : 0;
            }
        }
        else {
            delay = 0;
        }


    if(vpadError == 0 && ((vpad.btns_d | vpad.btns_h) & VPAD_BUTTON_HOME))
            break;

		usleep(20000);
    }

	free(voice1.samples);
	AXRegisterFrameCallback((void*)0); //kinda important to do
    AXQuit();
	MEM1_free(screenBuffer);
	screenBuffer = NULL;

    //!*******************************************************************
    //!                    Enter main application                        *
    //!*******************************************************************

    log_printf("Unmount SD\n");
    unmount_sd_fat("sd");
    log_printf("Release memory\n");
    memoryRelease();
    log_deinit();

    return EXIT_SUCCESS;
}

void playAlarm()
{
	voice1.newFrequency = (float)(440 * exp((-9 + 1) * log(2)/12));
    //clear the modified flag
    //request new sound
    voice1.newSoundRequested = 1;
    //clear the modified flag
    voice1.modified = 0;

    //flush everything to memory
    DCFlushRange(&voice1.newFrequency, sizeof(voice1.newFrequency));
    DCFlushRange(&voice1.newSoundRequested, sizeof(voice1.newSoundRequested));
    DCFlushRange(&voice1, sizeof(voice1));
}

#define AMPLITUDE 100

unsigned int generateSineWave(sample* samples, unsigned int maxLength, float freq) {
	int sinX = 0;
	float t = (M_PI * 2 * freq) / (48000); //freqHz sine @ 48KHz sample rate
	for (unsigned int i = 0; i < maxLength; i++) {
		if ((t * sinX) >= (2 * M_PI)) { //do the minimum number of samples
			return i - 1;
		}
		float sinResult = (float)(sin(t * sinX));
		samples[i] = (sample)(AMPLITUDE * sinResult);
		sinX++;
	}
	return maxLength;
}

ax_buffer_t voiceBuffer;

void axFrameCallback() {
	if (voice1.stopRequested) {
		//Stop the voice
		AXSetVoiceState(voice1.voice, 0);
		//Clear the flag
		voice1.stopRequested = 0;
		voice1.stopped = 1;
		DCFlushRange(&voice1, sizeof(voice1));
	} else if (voice1.modified) {
		//does this really need to happen in the callback?
		DCInvalidateRange(&voice1, sizeof(voice1));
		memset(&voiceBuffer, 0, sizeof(voiceBuffer));
		voiceBuffer.samples = voice1.samples;
		voiceBuffer.format = 25; //almost definitely wrong
		voiceBuffer.loop = 1;
		voiceBuffer.cur_pos = 0;
		voiceBuffer.end_pos = voice1.numSamples - 1;
		voiceBuffer.loop_offset = 0;

		unsigned int ratioBits[4];
		ratioBits[0] = (unsigned int)(0x00010000 * ((float)48000 / (float)AXGetInputSamplesPerSec()));
		ratioBits[1] = 0;
		ratioBits[2] = 0;
		ratioBits[3] = 0;

		AXSetVoiceOffsets(voice1.voice, &voiceBuffer);
		AXSetVoiceSrc(voice1.voice, ratioBits);
		AXSetVoiceSrcType(voice1.voice, 1);
		AXSetVoiceState(voice1.voice, 1);
		voice1.modified = 0;
		voice1.stopped = 0;
		DCFlushRange(&voice1, sizeof(voice1));
	}
}
