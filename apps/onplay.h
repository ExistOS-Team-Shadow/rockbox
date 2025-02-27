/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Björn Stenberg
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
#ifndef _ONPLAY_H_
#define _ONPLAY_H_

int onplay(char* file, int attr, int from_screen, bool hotkey);
int get_onplay_context(void);

enum {
    ONPLAY_MAINMENU = -1,
    ONPLAY_OK = 0,
    ONPLAY_RELOAD_DIR,
    ONPLAY_START_PLAY,
    ONPLAY_PLAYLIST,
    ONPLAY_PLUGIN,
#ifdef HAVE_HOTKEY
    ONPLAY_FUNC_RETURN, /* for use in hotkey_assignment only */
#endif
};

#ifdef HAVE_HOTKEY
int get_hotkey_lang_id(int action);

enum hotkey_action {
    HOTKEY_OFF = 0,
    HOTKEY_VIEW_PLAYLIST,
    HOTKEY_PROPERTIES,
#ifdef HAVE_TAGCACHE
    HOTKEY_PICTUREFLOW,
#endif
    HOTKEY_SHOW_TRACK_INFO,
    HOTKEY_PITCHSCREEN,
    HOTKEY_OPEN_WITH,
    HOTKEY_DELETE,
    HOTKEY_BOOKMARK,
    HOTKEY_PLUGIN,
    HOTKEY_INSERT,
    HOTKEY_INSERT_SHUFFLED,
};
#endif

/* needed for the playlist viewer.. eventually clean this up */
void onplay_show_playlist_cat_menu(char* track_name);
void onplay_show_playlist_menu(const char* path, void (*playlist_insert_cb));

#endif
