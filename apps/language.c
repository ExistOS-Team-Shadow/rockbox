/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Daniel Stenberg
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

#include <file.h>

#include "language.h"
#include "lang.h"
#include "debug.h"
#include "string.h"
#ifdef HAVE_LCD_BITMAP
#include "viewport.h"
#endif

/* The following header is generated by the build system and only defines
   MAX_LANGUAGE_SIZE to be the size of the largest currently available
   language! */
#include "max_language_size.h"

/* These defines must match the initial bytes in the binary lang file */
/* See tools/genlang (TODO: Use common include for both) */
#define LANGUAGE_COOKIE   0x1a
#define LANGUAGE_VERSION  0x05
#define LANGUAGE_FLAG_RTL 0x01

#define HEADER_SIZE 4

static unsigned char language_buffer[MAX_LANGUAGE_SIZE];

void lang_init(void)
{
    int i;
    unsigned char *ptr = (unsigned char *) language_builtin;
    
    for (i = 0; i < LANG_LAST_INDEX_IN_ARRAY; i++) {
        language_strings[i] = ptr;
        ptr += strlen((char *)ptr) + 1; /* advance pointer to next string */
    }
}

static unsigned char lang_options = 0;

int lang_is_rtl(void)
{
    return (lang_options & LANGUAGE_FLAG_RTL) != 0;
}

int lang_load(const char *filename)
{
    int fsize;
    int fd = open(filename, O_RDONLY);
    int retcode=0;
    unsigned char lang_header[HEADER_SIZE];
    if(fd < 0)
        return 1;
    fsize = filesize(fd) - HEADER_SIZE;
    if(fsize <= MAX_LANGUAGE_SIZE) {
        read(fd, lang_header, HEADER_SIZE);
        if((lang_header[0] == LANGUAGE_COOKIE) &&
           (lang_header[1] == LANGUAGE_VERSION) &&
           (lang_header[2] == TARGET_ID)) {
            read(fd, language_buffer, MAX_LANGUAGE_SIZE);
            unsigned char *ptr = language_buffer;
            int id;
            lang_init();                    /* initialize with builtin */

            while(fsize>3) {
                id = (ptr[0]<<8) | ptr[1];  /* get two-byte id */
                ptr+=2;                     /* pass the id */
                if(id < LANG_LAST_INDEX_IN_ARRAY) {
#if 0
                    DEBUGF("%2x New: %30s ", id, ptr);
                    DEBUGF("Replaces: %s\n", language_strings[id]);
#endif
                    language_strings[id] = ptr; /* point to this string */
                }
                while(*ptr) {               /* pass the string */
                    fsize--;
                    ptr++;
                }
                fsize-=3; /* the id and the terminating zero */
                ptr++;       /* pass the terminating zero-byte */
            }
        }
        else {
            DEBUGF("Illegal language file\n");
            retcode = 2;
        }
    }
    else {
        DEBUGF("Language %s too large: %d\n", filename, fsize);
        retcode = 3;
    }
    close(fd);
    if (retcode)
    {
        lang_options = 0;
    }
    else
    {
        lang_options = lang_header[3];
#ifdef HAVE_LCD_BITMAP
        viewportmanager_theme_changed(THEME_UI_VIEWPORT);
#endif
    }
    return retcode;
}

int lang_english_to_id(const char* english)
{
    int i;
    unsigned char *ptr = (unsigned char *) language_builtin;
    
    for (i = 0; i < LANG_LAST_INDEX_IN_ARRAY; i++) {
        if (!strcmp(ptr, english))
            return i;
        ptr += strlen((char *)ptr) + 1; /* advance pointer to next string */
    }
    return -1;
}
