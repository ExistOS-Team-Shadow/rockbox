/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * user intereface of image viewer.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef _IMAGE_VIEWER_BUTTONS_H
#define _IMAGE_VIEWER_BUTTONS_H

#include "plugin.h"

/* variable button definitions */
#if CONFIG_KEYPAD == RECORDER_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_PLAY
#define IMGVIEW_ZOOM_OUT    BUTTON_ON
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_F3
#define IMGVIEW_PREVIOUS    BUTTON_F2
#define IMGVIEW_MENU        BUTTON_OFF

#elif CONFIG_KEYPAD == ARCHOS_AV300_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_SELECT
#define IMGVIEW_ZOOM_OUT    BUTTON_ON
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_F3
#define IMGVIEW_PREVIOUS    BUTTON_F2
#define IMGVIEW_MENU        BUTTON_OFF

#elif CONFIG_KEYPAD == ONDIO_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_MENU
#define IMGVIEW_ZOOM_IN     (BUTTON_MENU | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_MENU | BUTTON_DOWN)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_MENU | BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_MENU | BUTTON_LEFT)
#define IMGVIEW_MENU        BUTTON_OFF

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define IMGVIEW_ZOOM_IN     BUTTON_SELECT
#define IMGVIEW_ZOOM_OUT    BUTTON_MODE
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#if (CONFIG_KEYPAD == IRIVER_H100_PAD)
#define IMGVIEW_NEXT        BUTTON_ON
#define IMGVIEW_PREVIOUS    BUTTON_REC
#else
#define IMGVIEW_NEXT        BUTTON_REC
#define IMGVIEW_PREVIOUS    BUTTON_ON
#endif
#define IMGVIEW_MENU        BUTTON_OFF
#define IMGVIEW_RC_MENU     BUTTON_RC_STOP

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)
#define IMGVIEW_ZOOM_IN     BUTTON_SCROLL_FWD
#define IMGVIEW_ZOOM_OUT    BUTTON_SCROLL_BACK
#define IMGVIEW_UP          BUTTON_MENU
#define IMGVIEW_DOWN        BUTTON_PLAY
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_SELECT | BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_SELECT | BUTTON_LEFT)
#define IMGVIEW_MENU        (BUTTON_SELECT | BUTTON_MENU)
#define IMGVIEW_QUIT        (BUTTON_SELECT | BUTTON_PLAY)

#elif CONFIG_KEYPAD == IAUDIO_X5M5_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_SELECT
#define IMGVIEW_ZOOM_IN     (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_PLAY
#define IMGVIEW_PREVIOUS    BUTTON_REC
#define IMGVIEW_MENU        BUTTON_POWER

#elif CONFIG_KEYPAD == GIGABEAT_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_A | BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_A | BUTTON_LEFT)
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == SANSA_E200_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_SCROLL_FWD
#define IMGVIEW_NEXT_REPEAT     (BUTTON_SCROLL_FWD|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_SCROLL_BACK
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_SCROLL_BACK|BUTTON_REPEAT)
#define IMGVIEW_MENU            BUTTON_POWER
#define IMGVIEW_SLIDE_SHOW      BUTTON_REC

#elif CONFIG_KEYPAD == SANSA_FUZE_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_SCROLL_FWD
#define IMGVIEW_NEXT_REPEAT     (BUTTON_SCROLL_FWD|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_SCROLL_BACK
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_SCROLL_BACK|BUTTON_REPEAT)
#define IMGVIEW_MENU            (BUTTON_HOME|BUTTON_REPEAT)

#elif CONFIG_KEYPAD == SANSA_C200_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_VOL_UP
#define IMGVIEW_NEXT_REPEAT     (BUTTON_VOL_UP|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_VOL_DOWN
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_VOL_DOWN|BUTTON_REPEAT)
#define IMGVIEW_MENU            BUTTON_POWER
#define IMGVIEW_SLIDE_SHOW      BUTTON_REC

#elif CONFIG_KEYPAD == SANSA_CLIP_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_VOL_UP
#define IMGVIEW_NEXT_REPEAT     (BUTTON_VOL_UP|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_VOL_DOWN
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_VOL_DOWN|BUTTON_REPEAT)
#define IMGVIEW_MENU            BUTTON_POWER
#define IMGVIEW_SLIDE_SHOW      BUTTON_HOME

#elif CONFIG_KEYPAD == SANSA_M200_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         (BUTTON_SELECT | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_SELECT | BUTTON_REPEAT)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_VOL_UP
#define IMGVIEW_NEXT_REPEAT     (BUTTON_VOL_UP|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_VOL_DOWN
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_VOL_DOWN|BUTTON_REPEAT)
#define IMGVIEW_MENU            BUTTON_POWER
#define IMGVIEW_SLIDE_SHOW      (BUTTON_SELECT | BUTTON_UP)

#elif CONFIG_KEYPAD == IRIVER_H10_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_PLAY
#define IMGVIEW_ZOOM_IN     (BUTTON_PLAY | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_PLAY | BUTTON_REPEAT)
#define IMGVIEW_UP          BUTTON_SCROLL_UP
#define IMGVIEW_DOWN        BUTTON_SCROLL_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_FF
#define IMGVIEW_PREVIOUS    BUTTON_REW
#define IMGVIEW_MENU        BUTTON_POWER

#elif CONFIG_KEYPAD == MROBE500_PAD
#define IMGVIEW_MENU        BUTTON_POWER

#elif CONFIG_KEYPAD == GIGABEAT_S_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_NEXT
#define IMGVIEW_PREVIOUS    BUTTON_PREV
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_BACK

#elif CONFIG_KEYPAD == MROBE100_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_SELECT
#define IMGVIEW_ZOOM_OUT    BUTTON_PLAY
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_DISPLAY | BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_DISPLAY | BUTTON_LEFT)
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == IAUDIO_M3_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_RC_PLAY
#define IMGVIEW_ZOOM_IN     (BUTTON_RC_PLAY|BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_RC_PLAY|BUTTON_REPEAT)
#define IMGVIEW_UP          BUTTON_RC_VOL_UP
#define IMGVIEW_DOWN        BUTTON_RC_VOL_DOWN
#define IMGVIEW_LEFT        BUTTON_RC_REW
#define IMGVIEW_RIGHT       BUTTON_RC_FF
#define IMGVIEW_NEXT        BUTTON_RC_MODE
#define IMGVIEW_PREVIOUS    BUTTON_RC_MENU
#define IMGVIEW_MENU        BUTTON_RC_REC

#elif CONFIG_KEYPAD == COWON_D2_PAD

#elif CONFIG_KEYPAD == IAUDIO67_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOLUP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOLDOWN
#define IMGVIEW_UP          BUTTON_STOP
#define IMGVIEW_DOWN        BUTTON_PLAY
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_PLAY|BUTTON_VOLUP)
#define IMGVIEW_PREVIOUS    (BUTTON_PLAY|BUTTON_VOLDOWN)
#define IMGVIEW_MENU        BUTTON_MENU

#elif CONFIG_KEYPAD == CREATIVEZVM_PAD

#define IMGVIEW_ZOOM_IN     BUTTON_PLAY
#define IMGVIEW_ZOOM_OUT    BUTTON_CUSTOM
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_SELECT
#define IMGVIEW_PREVIOUS    BUTTON_BACK
#define IMGVIEW_MENU        BUTTON_MENU

#elif CONFIG_KEYPAD == CREATIVE_ZENXFI3_PAD
#define IMGVIEW_ZOOM_IN         (BUTTON_VOL_UP|BUTTON_REL)
#define IMGVIEW_ZOOM_OUT        (BUTTON_VOL_DOWN|BUTTON_REL)
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_BACK
#define IMGVIEW_RIGHT           BUTTON_MENU
#define IMGVIEW_NEXT            (BUTTON_PLAY|BUTTON_MENU)
#define IMGVIEW_PREVIOUS        (BUTTON_PLAY|BUTTON_BACK)
#define IMGVIEW_MENU            BUTTON_POWER

#elif CONFIG_KEYPAD == PHILIPS_HDD1630_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_VIEW
#define IMGVIEW_PREVIOUS    BUTTON_PLAYLIST
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == PHILIPS_HDD6330_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_NEXT
#define IMGVIEW_PREVIOUS    BUTTON_PREV
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == PHILIPS_SA9200_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_PREV
#define IMGVIEW_RIGHT       BUTTON_NEXT
#define IMGVIEW_NEXT        BUTTON_RIGHT
#define IMGVIEW_PREVIOUS    BUTTON_LEFT
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == ONDAVX747_PAD
#elif CONFIG_KEYPAD == ONDAVX777_PAD

#elif CONFIG_KEYPAD == SAMSUNG_YH92X_PAD
#define IMGVIEW_ZOOM_IN     (BUTTON_PLAY|BUTTON_UP)
#define IMGVIEW_ZOOM_OUT    (BUTTON_PLAY|BUTTON_DOWN)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_FFWD
#define IMGVIEW_PREVIOUS    BUTTON_REW
#define IMGVIEW_NEXT_REPEAT (BUTTON_FFWD|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_REW|BUTTON_REPEAT)
#define IMGVIEW_MENU_PRE    BUTTON_PLAY
#define IMGVIEW_MENU        (BUTTON_PLAY|BUTTON_REL)
#define IMGVIEW_SLIDE_SHOW  BUTTON_REC_SW_ON
#define IMGVIEW_SLIDE_SHOW2 BUTTON_REC_SW_OFF

#elif CONFIG_KEYPAD == SAMSUNG_YH820_PAD
#define IMGVIEW_ZOOM_IN     (BUTTON_REC|BUTTON_UP)
#define IMGVIEW_ZOOM_OUT    (BUTTON_REC|BUTTON_DOWN)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_FFWD
#define IMGVIEW_PREVIOUS    BUTTON_REW
#define IMGVIEW_NEXT_REPEAT (BUTTON_FFWD|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_REW|BUTTON_REPEAT)
#define IMGVIEW_MENU_PRE    BUTTON_PLAY
#define IMGVIEW_MENU        (BUTTON_PLAY|BUTTON_REL)
#define IMGVIEW_SLIDE_SHOW  (BUTTON_PLAY|BUTTON_REPEAT)
#define IMGVIEW_SLIDE_SHOW_PRE BUTTON_PLAY
#define IMGVIEW_QUIT        (BUTTON_REC|BUTTON_PLAY)

#elif CONFIG_KEYPAD == PBELL_VIBE500_PAD
#define IMGVIEW_ZOOM_IN     (BUTTON_REC | BUTTON_UP)
#define IMGVIEW_ZOOM_OUT    (BUTTON_REC | BUTTON_DOWN)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_PREV
#define IMGVIEW_RIGHT       BUTTON_NEXT
#define IMGVIEW_NEXT        (BUTTON_REC | BUTTON_NEXT)
#define IMGVIEW_PREVIOUS    (BUTTON_REC | BUTTON_PREV)
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_CANCEL

#elif CONFIG_KEYPAD == MPIO_HD200_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_PLAY
#define IMGVIEW_ZOOM_OUT    BUTTON_REC
#define IMGVIEW_UP          (BUTTON_REC | BUTTON_REW)
#define IMGVIEW_DOWN        (BUTTON_REC | BUTTON_FF)
#define IMGVIEW_LEFT        BUTTON_VOL_DOWN
#define IMGVIEW_RIGHT       BUTTON_VOL_UP
#define IMGVIEW_NEXT        BUTTON_FF
#define IMGVIEW_PREVIOUS    BUTTON_REW
#define IMGVIEW_MENU        BUTTON_FUNC
#define IMGVIEW_QUIT        (BUTTON_REC | BUTTON_PLAY)

#elif CONFIG_KEYPAD == MPIO_HD300_PAD
#define IMGVIEW_ZOOM_IN     (BUTTON_PLAY | BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    BUTTON_REC
#define IMGVIEW_UP          (BUTTON_PLAY | BUTTON_UP)
#define IMGVIEW_DOWN        (BUTTON_PLAY | BUTTON_DOWN)
#define IMGVIEW_LEFT        BUTTON_REW
#define IMGVIEW_RIGHT       BUTTON_FF
#define IMGVIEW_NEXT        BUTTON_DOWN
#define IMGVIEW_PREVIOUS    BUTTON_UP
#define IMGVIEW_MENU        BUTTON_ENTER
#define IMGVIEW_QUIT        (BUTTON_MENU | BUTTON_REPEAT)

#elif CONFIG_KEYPAD == SANSA_FUZEPLUS_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_BOTTOMRIGHT
#define IMGVIEW_PREVIOUS    BUTTON_BOTTOMLEFT
#define IMGVIEW_MENU        BUTTON_SELECT
#define IMGVIEW_QUIT        BUTTON_BACK
#define IMGVIEW_SLIDE_SHOW  BUTTON_PLAYPAUSE

#elif CONFIG_KEYPAD == SANSA_CONNECT_PAD
#define IMGVIEW_ZOOM_PRE        BUTTON_SELECT
#define IMGVIEW_ZOOM_IN         BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT        BUTTON_VOL_DOWN
#define IMGVIEW_UP              BUTTON_UP
#define IMGVIEW_DOWN            BUTTON_DOWN
#define IMGVIEW_LEFT            BUTTON_LEFT
#define IMGVIEW_RIGHT           BUTTON_RIGHT
#define IMGVIEW_NEXT            BUTTON_SCROLL_FWD
#define IMGVIEW_NEXT_REPEAT     (BUTTON_SCROLL_FWD|BUTTON_REPEAT)
#define IMGVIEW_PREVIOUS        BUTTON_SCROLL_BACK
#define IMGVIEW_PREVIOUS_REPEAT (BUTTON_SCROLL_BACK|BUTTON_REPEAT)
#define IMGVIEW_MENU            BUTTON_POWER
#define IMGVIEW_SLIDE_SHOW      BUTTON_NEXT

#elif CONFIG_KEYPAD == SAMSUNG_YPR0_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_POWER
#define IMGVIEW_ZOOM_OUT    BUTTON_USER
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_SELECT
#define IMGVIEW_PREVIOUS    0xFFFFFFA //not used
#define IMGVIEW_MENU        BUTTON_MENU
#define IMGVIEW_QUIT        BUTTON_BACK

#elif CONFIG_KEYPAD == HM60X_PAD
#define IMGVIEW_ZOOM_IN     (BUTTON_POWER | BUTTON_UP)
#define IMGVIEW_ZOOM_OUT    (BUTTON_POWER | BUTTON_DOWN)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_POWER | BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_POWER | BUTTON_LEFT)
#define IMGVIEW_MENU        BUTTON_SELECT
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == HM801_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_PLAY
#define IMGVIEW_ZOOM_OUT    (BUTTON_POWER | BUTTON_PLAY)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        BUTTON_NEXT
#define IMGVIEW_PREVIOUS    BUTTON_PREV
#define IMGVIEW_MENU        BUTTON_SELECT
#define IMGVIEW_QUIT        BUTTON_POWER

#elif CONFIG_KEYPAD == SONY_NWZ_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_POWER
#define IMGVIEW_ZOOM_IN     (BUTTON_POWER|BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_POWER|BUTTON_REPEAT)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_NEXT        BUTTON_PLAY
#define IMGVIEW_PREVIOUS    BUTTON_BACK
#define IMGVIEW_MENU        (BUTTON_BACK|BUTTON_REPEAT)

#elif CONFIG_KEYPAD == CREATIVE_ZEN_PAD
#define IMGVIEW_ZOOM_PRE    BUTTON_PLAYPAUSE
#define IMGVIEW_ZOOM_IN     (BUTTON_PLAYPAUSE|BUTTON_REL)
#define IMGVIEW_ZOOM_OUT    (BUTTON_PLAYPAUSE|BUTTON_REPEAT)
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_NEXT        BUTTON_SHORTCUT
#define IMGVIEW_PREVIOUS    BUTTON_BACK
#define IMGVIEW_MENU        BUTTON_MENU

#elif CONFIG_KEYPAD == DX50_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOL_UP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOL_DOWN
#define IMGVIEW_NEXT        BUTTON_RIGHT
#define IMGVIEW_PREVIOUS    BUTTON_LEFT
#define IMGVIEW_QUIT        (BUTTON_POWER|BUTTON_REL)
#define IMGVIEW_MENU        (BUTTON_PLAY|BUTTON_REL)

#elif CONFIG_KEYPAD == CREATIVE_ZENXFI2_PAD
#define IMGVIEW_QUIT        BUTTON_POWER
#define IMGVIEW_MENU        BUTTON_MENU

#elif CONFIG_KEYPAD == AGPTEK_ROCKER_PAD
#define IMGVIEW_ZOOM_IN     BUTTON_VOLUP
#define IMGVIEW_ZOOM_OUT    BUTTON_VOLDOWN
#define IMGVIEW_UP          BUTTON_UP
#define IMGVIEW_DOWN        BUTTON_DOWN
#define IMGVIEW_LEFT        BUTTON_LEFT
#define IMGVIEW_RIGHT       BUTTON_RIGHT
#define IMGVIEW_NEXT        (BUTTON_SELECT|BUTTON_RIGHT)
#define IMGVIEW_PREVIOUS    (BUTTON_SELECT|BUTTON_LEFT)
#define IMGVIEW_QUIT        (BUTTON_POWER|BUTTON_REL)
#define IMGVIEW_MENU        (BUTTON_SELECT|BUTTON_REL)

#else
#error No keymap defined!
#endif

#ifdef HAVE_TOUCHSCREEN
#ifndef IMGVIEW_UP
#define IMGVIEW_UP          BUTTON_TOPMIDDLE
#endif
#ifndef IMGVIEW_DOWN
#define IMGVIEW_DOWN        BUTTON_BOTTOMMIDDLE
#endif
#ifndef IMGVIEW_LEFT
#define IMGVIEW_LEFT        BUTTON_MIDLEFT
#endif
#ifndef IMGVIEW_RIGHT
#define IMGVIEW_RIGHT       BUTTON_MIDRIGHT
#endif
#ifndef IMGVIEW_ZOOM_IN
#define IMGVIEW_ZOOM_IN     BUTTON_TOPRIGHT
#endif
#ifndef IMGVIEW_ZOOM_OUT
#define IMGVIEW_ZOOM_OUT    BUTTON_TOPLEFT
#endif
#ifndef IMGVIEW_MENU
#define IMGVIEW_MENU        (BUTTON_CENTER|BUTTON_REL)
#endif
#ifndef IMGVIEW_NEXT
#define IMGVIEW_NEXT        BUTTON_BOTTOMRIGHT
#endif
#ifndef IMGVIEW_PREVIOUS
#define IMGVIEW_PREVIOUS    BUTTON_BOTTOMLEFT
#endif
#endif

#endif /* _IMAGE_VIEWER_BUTTONS_H */
