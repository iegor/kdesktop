/*
  mapping from file and id to pixmap, and global player map
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WASKINMAPPING_H
#define __WASKINMAPPING_H
 

/**
   This file is not a header file in the normal sense.
   It define _directly_ variables, which are used in the 
   WaSkinLoader class.
   Its a bit black voodoo magic.
*/

/**
   The Skin file format was downloaded from the web.
   http://www.algonet.et.se/~daniel7

   Author: unknown
*/

#include "skinMap.h"
#include "waSkins.h"

static const SkinMap normalMapToGui[] = {
    {0, 0, 275, 116},                     // _WA_MAPPING_MAIN
    {16, 88, 23, 18},                     // _WA_MAPPING_CBUTTONS_PREV
    {16 + 1 * 23, 88, 23, 18},            // _WA_MAPPING_CBUTTONS_PLAY
    {16 + 2 * 23, 88, 23, 18},            // _WA_MAPPING_CBUTTONS_PAUSE
    {16 + 3 * 23, 88, 23, 18},            // _WA_MAPPING_CBUTTONS_STOP
    {16 + 4 * 23, 88, 22, 18},            // _WA_MAPPING_CBUTTONS_NEXT
    {136, 89, 22, 16},                    // _WA_MAPPING_CBUTTONS_EJECT
    {239, 41, 29, 12},                    // _WA_MAPPING_MONOSTER_STEREO
    {212, 41, 27, 12},                    // _WA_MAPPING_MONOSTER_MONO
    {210, 89, 28, 15},                    // _WA_MAPPING_REPEAT
    {164, 89, 46, 15},                    // _WA_MAPPING_SHUFFLE
    {242, 58, 23, 12},                    // _WA_MAPPING_PLAYLIST
    {219, 58, 23, 12},                    // _WA_MAPPING_EQ
    {107, 57, 68, 13},                    // _WA_MAPPING_VOLUME_BAR
    {0, 0, 14, 11},                       // _WA_MAPPING_VOLUME_SLIDER
    {177, 57, 38, 13},                    // _WA_MAPPING_BALANCE_BAR
    {0, 0, 14, 11},                       // _WA_MAPPING_BALANCE_SLIDER
    {24, 28, 11, 9},                      // _WA_MAPPING_PLAYPAUS
    {16, 72, 248, 10},                    // _WA_MAPPING_POS_BAR
    {0, 0, 29, 10},                       // _WA_MAPPING_POS_BAR_SLIDER
    {40, 26, 63, 13},                     // _WA_MAPPING_DIGITS
    {40, 32, 5, 1},                       // _WA_MAPPING_MINUS
    {48, 26, 9, 13},                      // _WA_MAPPING_DIGIT_1
    {60, 26, 9, 13},                      // _WA_MAPPING_DIGIT_2
    {78, 26, 9, 13},                      // _WA_MAPPING_DIGIT_3
    {90, 26, 9, 13},                      // _WA_MAPPING_DIGIT_4
    {24, 43, 75, 16},                     // _WA_MAPPING_ANALYSER
    {111, 43, 15, 6},                     // _WA_MAPPING_BPS
    {156, 43, 10, 6},                     // _WA_MAPPING_FREQ
    {112, 27, 152, 6},                    // _WA_MAPPING_INFO
    {0, 0, 274, 14},                      // _WA_MAPPING_TITLE
    {6, 3, 9, 9},                         // _WA_MAPPING_TITLE_MENU
    {244, 3, 9, 9},                       // _WA_MAPPING_TITLE_MIN
    {254, 3, 9, 9},                       // _WA_MAPPING_TITLE_SHADE
    {264, 3, 9, 9},                       // _WA_MAPPING_TITLE_CLOSE
    {10, 22, 8, 43}                       // _WA_MAPPING_CLUTTERBAR
};


static const SkinDesc normalMapFromFile[] = {   
    {_WA_FILE_MAIN, 0, 0, 275, 116},      // _WA_SKIN_MAIN
    {_WA_FILE_CBUTTONS, 0, 0, 23, 18},    // _WA_SKIN_CBUTTONS_PREV_NORM
    {_WA_FILE_CBUTTONS, 0, 18, 23, 18},   // _WA_SKIN_CBUTTONS_PREV_PRES
    {_WA_FILE_CBUTTONS, 23, 0, 23, 18},   // _WA_SKIN_CBUTTONS_PLAY_NORM,
    {_WA_FILE_CBUTTONS, 23, 18, 23, 18},  // _WA_SKIN_CBUTTONS_PLAY_PRES
    {_WA_FILE_CBUTTONS, 46, 0, 23, 18},   // _WA_SKIN_CBUTTONS_PAUSE_NORM
    {_WA_FILE_CBUTTONS, 46, 18, 23, 18},  // _WA_SKIN_CBUTTONS_PAUSE_PRES
    {_WA_FILE_CBUTTONS, 69, 0, 23, 18},   // _WA_SKIN_CBUTTONS_STOP_NORM
    {_WA_FILE_CBUTTONS, 69, 18, 23, 18},  // _WA_SKIN_CBUTTONS_STOP_PRES
    {_WA_FILE_CBUTTONS, 92, 0, 22, 18},   // _WA_SKIN_CBUTTONS_NEXT_NORM
    {_WA_FILE_CBUTTONS, 92, 18, 22, 18},  // _WA_SKIN_CBUTTONS_NEXT_PRES
    {_WA_FILE_CBUTTONS, 114, 0, 22, 16},  // _WA_SKIN_CBUTTONS_EJECT_NORM
    {_WA_FILE_CBUTTONS, 114, 16, 22, 16}, // _WA_SKIN_CBUTTONS_EJECT_PRESS
    {_WA_FILE_MONOSTER, 0, 0, 29, 12},    // _WA_SKIN_MONOSTER_STEREO_TRUE
    {_WA_FILE_MONOSTER, 0, 12, 29, 12},   // _WA_SKIN_MONOSTER_STEREO_FALSE
    {_WA_FILE_MONOSTER, 29, 0, 27, 12},   // _WA_SKIN_MONOSTER_MONO_TRUE
    {_WA_FILE_MONOSTER, 29, 12, 27, 12},  // _WA_SKIN_MONOSTER_MONO_FALSE
    {_WA_FILE_NUMBERS, 0, 0, 99, 13},     // _WA_SKIN_NUMBERS
    {_WA_FILE_NUMBERS, 20, 6, 5, 1},      // _WA_SKIN_NUMBERS_MINUS
    {_WA_FILE_NUMBERS, 90, 6, 5, 1},      // _WA_SKIN_NUMBERS_BLANK
    {_WA_FILE_SHUFREP, 0, 0, 28, 15},     // _WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM
    {_WA_FILE_SHUFREP, 0, 15, 28, 15},    // _WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES
    {_WA_FILE_SHUFREP, 0, 30, 28, 15},    // _WA_SKIN_SHUFREP_REPEAT_SET_NORM
    {_WA_FILE_SHUFREP, 0, 45, 28, 15},    // _WA_SKIN_SHUFREP_REPEAT_SET_PRES
    {_WA_FILE_SHUFREP, 28, 0, 47, 15},    // _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM
    {_WA_FILE_SHUFREP, 28, 15, 47, 15},   // _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES
    {_WA_FILE_SHUFREP, 28, 30, 47, 15},   // _WA_SKIN_SHUFREP_SHUFFLE_SET_NORM
    {_WA_FILE_SHUFREP, 28, 45, 47, 15},   // _WA_SKIN_SHUFREP_SHUFFLE_SET_PRES
    {_WA_FILE_SHUFREP, 23, 61, 23, 12},   // _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM
    {_WA_FILE_SHUFREP, 69, 61, 23, 12},   // _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES
    {_WA_FILE_SHUFREP, 23, 73, 23, 12},   // _WA_SKIN_SHUFREP_PLAYLIST_SET_NORM
    {_WA_FILE_SHUFREP, 73, 23, 12},       // _WA_SKIN_SHUFREP_PLAYLIST_SET_PRES
    {_WA_FILE_SHUFREP, 0, 61, 23, 12},    // _WA_SKIN_SHUFREP_EQ_NOT_SET_NORM
    {_WA_FILE_SHUFREP, 46, 61, 23, 12},   // _WA_SKIN_SHUFREP_EQ_NOT_SET_PRES
    {_WA_FILE_SHUFREP, 0, 73, 23, 12},    // _WA_SKIN_SHUFREP_EQ_SET_NORM
    {_WA_FILE_SHUFREP, 46, 73, 23, 12},   // _WA_SKIN_SHUFREP_EQ_SET_PRES
    {_WA_FILE_TEXT, 0, 0, 155, 18},       // _WA_SKIN_TEXT
    {_WA_FILE_VOLUME, 0, 0, 68, 421},     // _WA_SKIN_VOLUME_BAR_ALL_BARS
    {_WA_FILE_VOLUME, 0, 0, 68, 13},      // _WA_SKIN_VOLUME_BAR
    {_WA_FILE_VOLUME, 0, 422, 14, 11},    // _WA_SKIN_VOLUME_SLIDER_NORM
    {_WA_FILE_VOLUME, 15, 422, 14, 11},   // _WA_SKIN_VOLUME_SLIDER_PRES
    {_WA_FILE_BALANCE, 9, 0, 38, 421},    // _WA_SKIN_BALANCE_BAR_ALL_BARS
    {_WA_FILE_BALANCE, 9, 0, 38, 13},     // _WA_SKIN_BALANCE_BAR
    {_WA_FILE_BALANCE, 0, 422, 14, 11},   // _WA_SKIN_BALANCE_SLIDER_NORM
    {_WA_FILE_BALANCE, 15, 422, 14, 11},  // _WA_SKIN_BALANCE_SLIDER_PRES
    {_WA_FILE_POSBAR, 0, 0, 248, 10},     // _WA_SKIN_POS_BAR
    {_WA_FILE_POSBAR, 278, 0, 29, 10},    // _WA_SKIN_POS_BAR_SLIDER_NORM
    {_WA_FILE_POSBAR, 248, 0, 29, 10},    // _WA_SKIN_POS_BAR_SLIDER_PRES
    {_WA_FILE_PLAYPAUS, 1, 0, 8, 9},      // _WA_SKIN_PLAYPAUS_PLAY
    {_WA_FILE_PLAYPAUS, 9, 0, 9, 9},      // _WA_SKIN_PLAYPAUS_PAUSE
    {_WA_FILE_PLAYPAUS, 18, 0, 9, 9},     // _WA_SKIN_PLAYPAUS_STOP
    {_WA_FILE_PLAYPAUS, 27, 0, 2, 9},     // _WA_SKIN_PLAYPAUS_FILLER
    {_WA_FILE_PLAYPAUS, 36, 0, 3, 9},     // _WA_SKIN_PLAYPAUS_WORK_INDICATOR
    {_WA_FILE_TITLEBAR, 27, 0, 275, 14},  // _WA_SKIN_TITLE_ACTIVE
    {_WA_FILE_TITLEBAR, 27, 15, 275, 14}, // _WA_SKIN_TITLE_INACTIVE
    {_WA_FILE_TITLEBAR, 0, 9, 9, 9},      // _WA_SKIN_TITLE_MENU_PRES
    {_WA_FILE_TITLEBAR, 0, 0, 9, 9},      // _WA_SKIN_TITLE_MENU_NORM
    {_WA_FILE_TITLEBAR, 33, 18, 9, 9},    // _WA_SKIN_TITLE_MENU_INACTIVE
    {_WA_FILE_TITLEBAR, 9, 9, 9, 9},      // _WA_SKIN_TITLE_MIN_PRES
    {_WA_FILE_TITLEBAR, 9, 0, 9, 9},      // _WA_SKIN_TITLE_MIN_NORM
    {_WA_FILE_TITLEBAR, 271, 18, 9, 9},   // _WA_SKIN_TITLE_MIN_INACTIVE
    {_WA_FILE_TITLEBAR, 9, 18, 9, 9},     // _WA_SKIN_TITLE_SHADE_PRES
    {_WA_FILE_TITLEBAR, 0, 18, 9, 9},     // _WA_SKIN_TITLE_SHADE_NORM
    {_WA_FILE_TITLEBAR, 281, 18, 9, 9},   // _WA_SKIN_TITLE_SHADE_INACTIVE
    {_WA_FILE_TITLEBAR, 18, 9, 9, 9},     // _WA_SKIN_TITLE_CLOSE_PRES
    {_WA_FILE_TITLEBAR, 18, 0, 9, 9},     // _WA_SKIN_TITLE_CLOSE_NORM
    {_WA_FILE_TITLEBAR, 291, 18, 9, 9},   // _WA_SKIN_TITLE_CLOSE_INACTIVE
    {_WA_FILE_TITLEBAR, 312, 0, 8, 43}    // _WA_SKIN_CLUTTERBAR_DISABLED
};

#endif
