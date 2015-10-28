#ifndef __COLOR_PRINT_H__
#define __COLOR_PRINT_H__

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#else
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define TRUE true
#endif


#if defined(__cplusplus)
extern "C" {
#endif
    /* Colors */
    enum  cp_color {
#ifdef WINDOWS
        CP_FG_BLACK         = 0x0000,
        CP_FG_BLUE          = 0x0001,
        CP_FG_GREEN         = 0x0002,
        CP_FG_CYAN          = 0x0003,
        CP_FG_RED           = 0x0004,
        CP_FG_MAGENTA       = 0x0005,
        CP_FG_BROWN         = 0x0006,
        CP_FG_LIGHTGRAY     = 0x0007,
        CP_FG_GRAY          = 0x0008,
        CP_FG_LIGHTBLUE     = 0x0009,
        CP_FG_LIGHTGREEN    = 0x000a,
        CP_FG_LIGHTCYAN     = 0x000b,
        CP_FG_LIGHTRED      = 0x000c,
        CP_FG_LIGHTMAGENTA  = 0x000d,
        CP_FG_YELLOW        = 0x000e,
        CP_FG_WHITE         = 0x000f,

        CP_BG_BLUE          = BACKGROUND_BLUE,
        CP_BG_GREEN         = BACKGROUND_GREEN,
        CP_BG_RED           = BACKGROUND_RED,
        CP_BG_GRAY          = BACKGROUND_INTENSITY,
#else  // LINUX
        CP_FG_BLACK         = 0x0001,
        CP_FG_RED           = 0x0002,
        CP_FG_GREEN         = 0x0003,
        CP_FG_YELLOW        = 0x0004,
        CP_FG_BLUE          = 0x0005,
        CP_FG_MAGENTA       = 0x0006,
        CP_FG_CYAN          = 0x0007,
        CP_FG_WHITE         = 0x0008,
        CP_FG_NULL          = 0x000f,

        CP_BG_BLACK         = 0x0010,
        CP_BG_RED           = 0x0020,
        CP_BG_GREEN         = 0x0030,
        CP_BG_YELLOW        = 0x0040,
        CP_BG_BLUE          = 0x0050,
        CP_BG_MAGENTA       = 0x0060,
        CP_BG_GYAN          = 0x0070,
        CP_BG_WHITE         = 0x0080,
        CP_BG_NULL          = 0x00f0,
#endif
        CP_DEF              = 0x00ff,
    };

    /* Color-Print State */
    struct cp_state
    {
        short    default_color;
        short    current_color;
#ifdef WINDOWS
        HANDLE   std_output;
#else
        FILE*    std_output;
#endif
    };
    typedef struct cp_state* cp_state_ptr;

    /* Module Functions */
    cp_state_ptr cp_init();
    int  cp_print(cp_state_ptr, enum cp_color, const char * );
    void cp_reset(cp_state_ptr );
    void cp_close(cp_state_ptr );

#if defined(__cplusplus)
}
#endif

#endif /* __COLOR_PRINT_H__ */
