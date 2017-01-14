//WARNING: Flashing text

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
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

//Custom print define
#define PRINTU(x, y, ...) { snprintf(msg, 80, __VA_ARGS__); OSScreenPutFontEx(0, x, y, msg); OSScreenPutFontEx(1, x, y, msg); }


// Entry point
int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    InitOSFunctionPointers();
    InitSocketFunctionPointers();

    log_init("192.168.1.12");
    log_print("Starting launcher\n");

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

    char msg[80];
    uint8_t sel_digit = 1;
    uint8_t alarmTime[2];
    int vpadError = -1;
    int delay = 0;
    VPADData vpad;

    while(1)
    {
        VPADRead(0, &vpad, 1, &vpadError);

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

        //TODO: add sound
        if (alarmTime[1] == currentMinute && alarmTime[0] == currentHour){
            OSScreenClearBufferEx(0, 0);
            OSScreenClearBufferEx(1, 0);

            PRINTU(0, 0, "Wake up!");
            PRINTU(9, 0, "Wake up!");
            PRINTU(18, 0, "Wake up!");
            PRINTU(0, 1, "Wake up!");
            PRINTU(9, 1, "Wake up!");
            PRINTU(18, 1, "Wake up!");
            PRINTU(0, 2, "Wake up!");
            PRINTU(9, 2, "Wake up!");
            PRINTU(18, 2, "Wake up!");
            PRINTU(0, 4, "Wake up!");
            PRINTU(9, 4, "Wake up!");
            PRINTU(18, 4, "Wake up!");
            PRINTU(0, 5, "Wake up!");
            PRINTU(9, 5, "Wake up!");
            PRINTU(18, 5, "Wake up!");
            PRINTU(0, 6, "Wake up!");
            PRINTU(9, 6, "Wake up!");
            PRINTU(18, 6, "Wake up!");
            PRINTU(0, 7, "Wake up!");
            PRINTU(9, 7, "Wake up!");
            PRINTU(18, 7, "Wake up!");
            PRINTU(0, 8, "Wake up!");
            PRINTU(9, 8, "Wake up!");
            PRINTU(18, 8, "Wake up!");
            PRINTU(0, 9, "Wake up!");
            PRINTU(9, 9, "Wake up!");
            PRINTU(18, 9, "Wake up!");
            PRINTU(0, 10, "Wake up!");
            PRINTU(9, 10, "Wake up!");
            PRINTU(18, 10, "Wake up!");
            PRINTU(0, 11, "Wake up!");
            PRINTU(9, 11, "Wake up!");
            PRINTU(18, 11, "Wake up!");
            PRINTU(0, 12, "Wake up!");
            PRINTU(9, 12, "Wake up!");
            PRINTU(18, 12, "Wake up!");
            PRINTU(0, 13, "Wake up!");
            PRINTU(9, 13, "Wake up!");
            PRINTU(18, 13, "Wake up!");
            PRINTU(0, 14, "Wake up!");
            PRINTU(9, 14, "Wake up!");
            PRINTU(18, 14, "Wake up!");
            PRINTU(0, 0, "Wake up!");
            PRINTU(9, 0, "Wake up!");
            PRINTU(18, 0, "Wake up!");
            PRINTU(27, 0, "Wake up!");
            PRINTU(36, 0, "Wake up!");
            PRINTU(0, 1, "Wake up!");
            PRINTU(9, 1, "Wake up!");
            PRINTU(18, 1, "Wake up!");
            PRINTU(27, 1, "Wake up!");
            PRINTU(36, 1, "Wake up!");
            PRINTU(0, 2, "Wake up!");
            PRINTU(9, 2, "Wake up!");
            PRINTU(18, 2, "Wake up!");
            PRINTU(27, 2, "Wake up!");
            PRINTU(36, 2, "Wake up!");
            PRINTU(0, 3, "Wake up!");
            PRINTU(9, 3, "Wake up!");
            PRINTU(18, 3, "Wake up!");
            PRINTU(27, 3, "Wake up!");
            PRINTU(36, 3, "Wake up!");
            PRINTU(0, 4, "Wake up!");
            PRINTU(9, 4, "Wake up!");
            PRINTU(18, 4, "Wake up!");
            PRINTU(27, 4, "Wake up!");
            PRINTU(36, 4, "Wake up!");
            PRINTU(0, 5, "Wake up!");
            PRINTU(9, 5, "Wake up!");
            PRINTU(18, 5, "Wake up!");
            PRINTU(27, 5, "Wake up!");
            PRINTU(36, 5, "Wake up!");
            PRINTU(0, 6, "Wake up!");
            PRINTU(9, 6, "Wake up!");
            PRINTU(18, 6, "Wake up!");
            PRINTU(27, 6, "Wake up!");
            PRINTU(36, 6, "Wake up!");
            PRINTU(0, 7, "Wake up!");
            PRINTU(9, 7, "Wake up!");
            PRINTU(18, 7, "Wake up!");
            PRINTU(27, 7, "Wake up!");
            PRINTU(36, 7, "Wake up!");
            PRINTU(0, 8, "Wake up!");
            PRINTU(9, 8, "Wake up!");
            PRINTU(18, 8, "Wake up!");
            PRINTU(27, 8, "Wake up!");
            PRINTU(36, 8, "Wake up!");
            PRINTU(0, 9, "Wake up!");
            PRINTU(9, 9, "Wake up!");
            PRINTU(18, 9, "Wake up!");
            PRINTU(27, 9, "Wake up!");
            PRINTU(36, 9, "Wake up!");
            PRINTU(0, 10, "Wake up!");
            PRINTU(9, 10, "Wake up!");
            PRINTU(18, 10, "Wake up!");
            PRINTU(27, 10, "Wake up!");
            PRINTU(36, 10, "Wake up!");
            PRINTU(0, 11, "Wake up!");
            PRINTU(9, 11, "Wake up!");
            PRINTU(18, 11, "Wake up!");
            PRINTU(27, 11, "Wake up!");
            PRINTU(36, 11, "Wake up!");
            PRINTU(0, 12, "Wake up!");
            PRINTU(9, 12, "Wake up!");
            PRINTU(18, 12, "Wake up!");
            PRINTU(27, 12, "Wake up!");
            PRINTU(36, 12, "Wake up!");
            PRINTU(0, 13, "Wake up!");
            PRINTU(9, 13, "Wake up!");
            PRINTU(18, 13, "Wake up!");
            PRINTU(27, 13, "Wake up!");
            PRINTU(36, 13, "Wake up!");
            PRINTU(0, 14, "Wake up!");
            PRINTU(9, 14, "Wake up!");
            PRINTU(18, 14, "Wake up!");
            PRINTU(27, 14, "Wake up!");
            PRINTU(36, 14, "Wake up!");

            OSScreenFlipBuffersEx(0);
            OSScreenFlipBuffersEx(1);

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

