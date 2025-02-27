/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002-2007 Björn Stenberg
 * Copyright (C) 2007-2008 Nicolas Pennequin
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
#include "font.h"
#include <stdio.h>
#include "string-extra.h"
#include <stdlib.h>
#include "action.h"
#include "system.h"
#include "settings.h"
#include "settings_list.h"
#include "rbunicode.h"
#include "timefuncs.h"
#include "status.h"
#include "power.h"
#include "powermgmt.h"
#include "sound.h"
#include "debug.h"
#include "cuesheet.h"
#include "replaygain.h"
#include "core_alloc.h"
#include "abrepeat.h"
#include "lang.h"
#include "misc.h"
#include "led.h"
#include "peakmeter.h"
/* Image stuff */
#include "albumart.h"
#include "playlist.h"
#include "playback.h"
#include "tdspeed.h"
#include "viewport.h"
#include "tagcache.h"

#include "wps_internals.h"
#include "skin_engine.h"
#include "statusbar-skinned.h"
#include "root_menu.h"
#ifdef HAVE_RECORDING
#include "recording.h"
#include "pcm_record.h"
#endif
#include "language.h"
#include "usb.h"
#if CONFIG_TUNER
#include "radio.h"
#include "tuner.h"
#include "fixedpoint.h"
#endif
#include "list.h"
#include "wps.h"

#define NOINLINE __attribute__ ((noinline))

static const char* get_codectype(const struct mp3entry* id3)
{
    if (id3 && id3->codectype < AFMT_NUM_CODECS) {
        return audio_formats[id3->codectype].label;
    } else {
        return NULL;
    }
}

/* Extract a part from a path.
 *
 * buf      - buffer extract part to.
 * buf_size - size of buffer.
 * path     - path to extract from.
 * level    - what to extract. 0 is file name, 1 is parent of file, 2 is
 *            parent of parent, etc.
 *
 * Returns buf if the desired level was found, NULL otherwise.
 */
char* get_dir(char* buf, int buf_size, const char* path, int level)
{
    const char* sep;
    const char* last_sep;
    int len;

    sep = path + strlen(path);
    last_sep = sep;

    while (sep > path)
    {
        if ('/' == *(--sep))
        {
            if (!level)
                break;

            level--;
            last_sep = sep - 1;
        }
    }

    if (level || (last_sep <= sep))
        return NULL;

    len = MIN(last_sep - sep, buf_size - 1);
    strmemccpy(buf, sep + 1, len + 1);
    return buf;
}

#if defined (HAVE_PITCHCONTROL)
/* A helper to determine the enum value for pitch/speed.

   When there are two choices (i.e. boolean), return 1 if the value is
   different from normal value and 2 if the value is the same as the
   normal value.  E.g. "%?Sp<%Sp>" would show the pitch only when
   playing at a modified pitch.

   When there are more than two choices (i.e. enum), the left half of
   the choices are to show 0..normal range, and the right half of the
   choices are to show values over that.  The last entry is used when
   it is set to the normal setting, following the rockbox convention
   to use the last entry for special values.

   E.g.

   2 items: %?Sp<0..99 or 101..infinity|100>
   3 items: %?Sp<0..99|101..infinity|100>
   4 items: %?Sp<0..49|50..99|101..infinity|100>
   5 items: %?Sp<0..49|50..99|101..149|150..infinity|100>
   6 items: %?Sp<0..33|34..66|67..99|101..133|134..infinity|100>
   7 items: %?Sp<0..33|34..66|67..99|101..133|134..167|167..infinity|100>
*/
static int pitch_speed_enum(int range, int32_t val, int32_t normval)
{
    int center;
    int n;

    if (range < 3)
        return (val == normval) + 1;
    if (val == normval)
        return range;
    center = range / 2;
    n = (center * val) / normval + 1;
    return (range <= n) ? (range - 1) : n;
}
#endif

const char *get_cuesheetid3_token(struct wps_token *token, struct mp3entry *id3,
                                  int offset_tracks, char *buf, int buf_size)
{
    struct cuesheet *cue = id3?id3->cuesheet:NULL;
    if (!cue || !cue->curr_track)
        return NULL;

    struct cue_track_info *track = cue->curr_track;
    if (offset_tracks)
    {
        if (cue->curr_track_idx+offset_tracks < cue->track_count)
            track+=offset_tracks;
        else
            return NULL;
    }
    switch (token->type)
    {
        case SKIN_TOKEN_METADATA_ARTIST:
            return *track->performer ? track->performer : NULL;
        case SKIN_TOKEN_METADATA_COMPOSER:
            return *track->songwriter ? track->songwriter : NULL;
        case SKIN_TOKEN_METADATA_ALBUM:
            return *cue->title ? cue->title : NULL;
        case SKIN_TOKEN_METADATA_ALBUM_ARTIST:
            return *cue->performer ? cue->performer : NULL;
        case SKIN_TOKEN_METADATA_TRACK_TITLE:
            return *track->title ? track->title : NULL;
        case SKIN_TOKEN_METADATA_TRACK_NUMBER:
            snprintf(buf, buf_size, "%d/%d",
                     cue->curr_track_idx+offset_tracks+1, cue->track_count);
            return buf;
        default:
            return NULL;
    }
    return NULL;
}

static const char* get_filename_token(struct wps_token *token, char* filename,
                               char *buf, int buf_size)
{
    if (filename)
    {
        switch (token->type)
        {
            case SKIN_TOKEN_FILE_PATH:
                return filename;
            case SKIN_TOKEN_FILE_NAME:
                if (get_dir(buf, buf_size, filename, 0)) {
                    /* Remove extension */
                    char* sep = strrchr(buf, '.');
                    if (NULL != sep) {
                        *sep = 0;
                    }
                    return buf;
                }
                return NULL;
            case SKIN_TOKEN_FILE_NAME_WITH_EXTENSION:
                return get_dir(buf, buf_size, filename, 0);
            case SKIN_TOKEN_FILE_DIRECTORY:
                return get_dir(buf, buf_size, filename, token->value.i);
            default:
                return NULL;
        }
    }
    return NULL;
}

/* All tokens which only need the info to return a value go in here */
const char *get_id3_token(struct wps_token *token, struct mp3entry *id3,
                          char *filename, char *buf, int buf_size, int limit, int *intval)
{
    struct wps_state *state = get_wps_state();
    if (id3)
    {
        unsigned long length = id3->length;
        unsigned long elapsed = id3->elapsed + state->ff_rewind_count;
        switch (token->type)
        {
            case SKIN_TOKEN_METADATA_ARTIST:
                return id3->artist;
            case SKIN_TOKEN_METADATA_COMPOSER:
                return id3->composer;
            case SKIN_TOKEN_METADATA_ALBUM:
                return id3->album;
            case SKIN_TOKEN_METADATA_ALBUM_ARTIST:
                return id3->albumartist;
            case SKIN_TOKEN_METADATA_GROUPING:
                return id3->grouping;
            case SKIN_TOKEN_METADATA_GENRE:
                return id3->genre_string;
            case SKIN_TOKEN_METADATA_DISC_NUMBER:
                if (id3->disc_string)
                    return id3->disc_string;
                if (id3->discnum) {
                    snprintf(buf, buf_size, "%d", id3->discnum);
                    return buf;
                }
                return NULL;
            case SKIN_TOKEN_METADATA_TRACK_NUMBER:
                if (id3->track_string)
                    return id3->track_string;
                if (id3->tracknum) {
                    snprintf(buf, buf_size, "%d", id3->tracknum);
                    return buf;
                }
                return NULL;
            case SKIN_TOKEN_METADATA_TRACK_TITLE:
                return id3->title;
            case SKIN_TOKEN_METADATA_VERSION:
                switch (id3->id3version)
                {
                    case ID3_VER_1_0:
                        return "1";
                    case ID3_VER_1_1:
                        return "1.1";
                    case ID3_VER_2_2:
                        return "2.2";
                    case ID3_VER_2_3:
                        return "2.3";
                    case ID3_VER_2_4:
                        return "2.4";
                    default:
                        break;
                }
                return NULL;
            case SKIN_TOKEN_METADATA_YEAR:
                if( id3->year_string )
                    return id3->year_string;
                if (id3->year) {
                    snprintf(buf, buf_size, "%d", id3->year);
                    return buf;
                }
                return NULL;
            case SKIN_TOKEN_METADATA_COMMENT:
                return id3->comment;
            case SKIN_TOKEN_FILE_BITRATE:
                if(id3->bitrate)
                    snprintf(buf, buf_size, "%d", id3->bitrate);
                else
                    return "?";
                return buf;
            case SKIN_TOKEN_TRACK_TIME_ELAPSED:
                if (intval && limit == TOKEN_VALUE_ONLY)
                    *intval = elapsed/1000;
                format_time(buf, buf_size, elapsed);
                return buf;

            case SKIN_TOKEN_TRACK_TIME_REMAINING:
                if (intval && limit == TOKEN_VALUE_ONLY)
                    *intval = (length - elapsed)/1000;
                format_time(buf, buf_size, length - elapsed);
                return buf;

            case SKIN_TOKEN_TRACK_LENGTH:
                if (intval && limit == TOKEN_VALUE_ONLY)
                    *intval = length/1000;
                format_time(buf, buf_size, length);
                return buf;

            case SKIN_TOKEN_TRACK_ELAPSED_PERCENT:
                if (length <= 0)
                    return NULL;

                if (intval)
                {
                    if (limit == TOKEN_VALUE_ONLY)
                        limit = 100; /* make it a percentage */
                    *intval = limit * elapsed / length + 1;
                }
                snprintf(buf, buf_size, "%lu", 100 * elapsed / length);
                return buf;

            case SKIN_TOKEN_TRACK_STARTING:
                {
                    unsigned long time = token->value.i * (HZ/TIMEOUT_UNIT);
                    if (elapsed < time)
                        return "starting";
                }
                return NULL;
            case SKIN_TOKEN_TRACK_ENDING:
                {
                    unsigned long time = token->value.i * (HZ/TIMEOUT_UNIT);
                    if (length - elapsed < time)
                        return "ending";
                }
                return NULL;

            case SKIN_TOKEN_FILE_CODEC:
                if (intval)
                {
                    if(id3->codectype == AFMT_UNKNOWN)
                        *intval = AFMT_NUM_CODECS;
                    else
                        *intval = id3->codectype;
                }
                return get_codectype(id3);

            case SKIN_TOKEN_FILE_FREQUENCY:
                snprintf(buf, buf_size, "%ld", id3->frequency);
                return buf;
            case SKIN_TOKEN_FILE_FREQUENCY_KHZ:
                /* ignore remainders < 100, so 22050 Hz becomes just 22k */
                if ((id3->frequency % 1000) < 100)
                    snprintf(buf, buf_size, "%ld", id3->frequency / 1000);
                else
                    snprintf(buf, buf_size, "%ld.%lu",
                            id3->frequency / 1000,
                            (id3->frequency % 1000) / 100);
                return buf;
            case SKIN_TOKEN_FILE_VBR:
                return (id3->vbr) ? "(avg)" : NULL;
            case SKIN_TOKEN_FILE_SIZE:
                snprintf(buf, buf_size, "%ld", id3->filesize / 1024);
                return buf;

#ifdef HAVE_TAGCACHE
        case SKIN_TOKEN_DATABASE_PLAYCOUNT:
            if (intval)
                *intval = id3->playcount + 1;
            snprintf(buf, buf_size, "%ld", id3->playcount);
            return buf;
        case SKIN_TOKEN_DATABASE_RATING:
            if (intval)
                *intval = id3->rating + 1;
            snprintf(buf, buf_size, "%d", id3->rating);
            return buf;
        case SKIN_TOKEN_DATABASE_AUTOSCORE:
            if (intval)
                *intval = id3->score + 1;
            snprintf(buf, buf_size, "%d", id3->score);
            return buf;
#endif

            default:
                return get_filename_token(token, id3->path, buf, buf_size);
        }
    }
    else /* id3 == NULL, handle the error based on the expected return type */
    {
        switch (token->type)
        {
            /* Most tokens expect NULL on error so leave that for the default case,
             * The ones that expect "0" need to be handled */
            case SKIN_TOKEN_FILE_FREQUENCY:
            case SKIN_TOKEN_FILE_FREQUENCY_KHZ:
            case SKIN_TOKEN_FILE_SIZE:
#ifdef HAVE_TAGCACHE
            case SKIN_TOKEN_DATABASE_PLAYCOUNT:
            case SKIN_TOKEN_DATABASE_RATING:
            case SKIN_TOKEN_DATABASE_AUTOSCORE:
#endif
                if (intval)
                    *intval = 0;
                return "0";
            default:
                return get_filename_token(token, filename, buf, buf_size);
        }
    }
    return buf;
}

#if CONFIG_TUNER

/* Formats the frequency (specified in Hz) in MHz,   */
/* with one or two digits after the decimal point -- */
/* depending on the frequency changing step.         */
/* Returns buf                                       */
static char *format_freq_MHz(int freq, int freq_step, char *buf, int buf_size)
{
    int decimals = (freq_step < 100000) + 1;
    int scale = ipow(10, 6 - decimals);
    int div = 1000000 / scale;
    freq = freq / scale;
    snprintf(buf, buf_size, "%d.%.*d", freq/div, decimals, freq%div);
    return buf;
}


/* Tokens which are really only used by the radio screen go in here */
const char *get_radio_token(struct wps_token *token, int preset_offset,
                            char *buf, int buf_size, int limit, int *intval)
{
    const struct fm_region_data *region_data =
            &(fm_region_data[global_settings.fm_region]);
    (void)limit;
    switch (token->type)
    {
        /* Radio/tuner tokens */
        case SKIN_TOKEN_TUNER_TUNED:
            if (tuner_get(RADIO_TUNED))
                return "t";
            return NULL;
        case SKIN_TOKEN_TUNER_SCANMODE:
            if (radio_scan_mode())
                return "s";
            return NULL;
        case SKIN_TOKEN_TUNER_STEREO:
            if (radio_is_stereo())
                return "s";
            return NULL;
        case SKIN_TOKEN_TUNER_MINFREQ: /* changes based on "region" */
            return format_freq_MHz(region_data->freq_min,
                            region_data->freq_step, buf, buf_size);
        case SKIN_TOKEN_TUNER_MAXFREQ: /* changes based on "region" */
            return format_freq_MHz(region_data->freq_max,
                            region_data->freq_step, buf, buf_size);
        case SKIN_TOKEN_TUNER_CURFREQ:
            return format_freq_MHz(radio_current_frequency(),
                            region_data->freq_step, buf, buf_size);
#ifdef HAVE_RADIO_RSSI
        case SKIN_TOKEN_TUNER_RSSI:
            snprintf(buf, buf_size, "%d",tuner_get(RADIO_RSSI));
            if (intval)
            {
                int val = tuner_get(RADIO_RSSI);
                int min = tuner_get(RADIO_RSSI_MIN);
                int max = tuner_get(RADIO_RSSI_MAX);
                if (limit == TOKEN_VALUE_ONLY)
                {
                    *intval = val;
                }
                else
                {
                    *intval = 1+(limit-1)*(val-min)/(max-1-min);
                }
            }
            return buf;
        case SKIN_TOKEN_TUNER_RSSI_MIN:
            snprintf(buf, buf_size, "%d",tuner_get(RADIO_RSSI_MIN));
            return buf;
        case SKIN_TOKEN_TUNER_RSSI_MAX:
            snprintf(buf, buf_size, "%d",tuner_get(RADIO_RSSI_MAX));
            return buf;
#endif
        case SKIN_TOKEN_PRESET_NAME:
        case SKIN_TOKEN_PRESET_FREQ:
        case SKIN_TOKEN_PRESET_ID:
        {
            int preset_count = radio_preset_count();
            int cur_preset = radio_current_preset();
            if (preset_count == 0 || cur_preset < 0)
                return NULL;
            int preset = cur_preset + preset_offset;
            /* make sure it's in the valid range */
            preset %= preset_count;
            if (preset < 0)
                preset += preset_count;
            if (token->type == SKIN_TOKEN_PRESET_NAME)
                snprintf(buf, buf_size, "%s", radio_get_preset(preset)->name);
            else if (token->type == SKIN_TOKEN_PRESET_FREQ)
                format_freq_MHz(radio_get_preset(preset)->frequency,
                                region_data->freq_step, buf, buf_size);
            else
                snprintf(buf, buf_size, "%d", preset + 1);
            return buf;
        }
        case SKIN_TOKEN_PRESET_COUNT:
            snprintf(buf, buf_size, "%d", radio_preset_count());
            if (intval)
                *intval = radio_preset_count();
            return buf;
        case SKIN_TOKEN_HAVE_RDS:
#ifdef HAVE_RDS_CAP
            return "rds";
        case SKIN_TOKEN_RDS_NAME:
            tuner_get_rds_info(RADIO_RDS_NAME, buf, buf_size);
            return buf;
        case SKIN_TOKEN_RDS_TEXT:
            tuner_get_rds_info(RADIO_RDS_TEXT, buf, buf_size);
            return buf;
#else
            return NULL; /* end of the SKIN_TOKEN_HAVE_RDS case */
#endif /* HAVE_RDS_CAP */
        default:
            return NULL;
    }
    return NULL;
}
#endif

static struct mp3entry* get_mp3entry_from_offset(int offset, char **filename)
{
    struct mp3entry* pid3 = NULL;
    struct wps_state *state = get_wps_state();
    struct cuesheet *cue = state->id3 ? state->id3->cuesheet : NULL;
    const char *fname = NULL;
    if (cue && cue->curr_track_idx + offset < cue->track_count)
        pid3 = state->id3;
    else if (offset == 0)
        pid3 = state->id3;
    else if (offset == 1)
        pid3 = state->nid3;
    else
    {
        static char filename_buf[MAX_PATH + 1];
        fname = playlist_peek(offset, filename_buf, sizeof(filename_buf));
        *filename = (char*)fname;
        static struct mp3entry tempid3;
        if (
#if defined(HAVE_TC_RAMCACHE) && defined(HAVE_DIRCACHE)
            tagcache_fill_tags(&tempid3, fname) ||
#endif
            audio_peek_track(&tempid3, offset)
        )
        {
            pid3 = &tempid3;
        }
    }
    return pid3;
}

/* Don't inline this; it was broken out of get_token_value to reduce stack
 * usage.
 */
static const char* NOINLINE get_lif_token_value(struct gui_wps *gwps,
                                                struct logical_if *lif,
                                                int offset, char *buf,
                                                int buf_size)
{
    int a = lif->num_options;
    int b;
    bool number_set = true;
    struct wps_token *liftoken = SKINOFFSETTOPTR(get_skin_buffer(gwps->data), lif->token);
    const char* out_text = get_token_value(gwps, liftoken, offset, buf, buf_size, &a);
    if (a == -1 && liftoken->type != SKIN_TOKEN_VOLUME)
    {
        a = (out_text && *out_text) ? 1 : 0;
        number_set = false;
    }
    switch (lif->operand.type)
    {
        case STRING:
        {
            char *cmp = SKINOFFSETTOPTR(get_skin_buffer(gwps->data), lif->operand.data.text);
            if (out_text == NULL)
                return NULL;
            a = strcmp(out_text, cmp);
            b = 0;
            break;
        }
        case INTEGER:
            if (!number_set && out_text && ((*out_text >= '0' && *out_text <= '9') || (*out_text == '-' && out_text[1] >= '0' && out_text[1] <= '9')))
                a = atoi(out_text);
            /* fall through */
        case PERCENT:
        case DECIMAL:
            b = lif->operand.data.number;
            break;
        case CODE:
        {
            char temp_buf[MAX_PATH];
            const char *outb;
            struct skin_element *element = SKINOFFSETTOPTR(get_skin_buffer(gwps->data), lif->operand.data.code);
            if (!element) return NULL;
            struct wps_token *token = SKINOFFSETTOPTR(get_skin_buffer(gwps->data), element->data);
            b = lif->num_options;

            outb = get_token_value(gwps, token, offset, temp_buf,
                                   sizeof(temp_buf), &b);

            if (b == -1 && liftoken->type != SKIN_TOKEN_VOLUME)
            {
                if (!out_text || !outb)
                    return (lif->op == IF_EQUALS) ? NULL : "neq";
                bool equal = strcmp(out_text, outb) == 0;
                if (lif->op == IF_EQUALS)
                    return equal ? "eq" : NULL;
                else if (lif->op == IF_NOTEQUALS)
                    return !equal ? "neq" : NULL;
                else
                    b = (outb && *outb) ? 1 : 0;
            }
        }
        break;
        case DEFAULT:
            b = -1;
            break;
    }

    switch (lif->op)
    {
        case IF_EQUALS:
            return a == b ? "eq" : NULL;
        case IF_NOTEQUALS:
            return a != b ? "neq" : NULL;
        case IF_LESSTHAN:
            return a < b ? "lt" : NULL;
        case IF_LESSTHAN_EQ:
            return a <= b ? "lte" : NULL;
        case IF_GREATERTHAN:
            return a > b ? "gt" : NULL;
        case IF_GREATERTHAN_EQ:
            return a >= b ? "gte" : NULL;
    }
    return NULL;
}

/* Return the tags value as text. buf should be used as temp storage if needed.

   intval is used with conditionals/enums: when this function is called,
   intval should contain the number of options in the conditional/enum.
   When this function returns, intval is -1 if the tag is non numeric or,
   if the tag is numeric, *intval is the enum case we want to go to (between 1
   and the original value of *intval, inclusive).
   When not treating a conditional/enum, intval should be NULL.
*/
const char *get_token_value(struct gui_wps *gwps,
                           struct wps_token *token, int offset,
                           char *buf, int buf_size,
                           int *intval)
{
    int numeric_ret = -1;
    const char *numeric_buf = "?";
    int fmt_size;

    if (!gwps)
        return NULL;
    if (!token)
        return NULL;

    struct wps_data *data = gwps->data;
    struct wps_state *state = get_wps_state();
    struct mp3entry *id3; /* Think very carefully about using this.
                             maybe get_id3_token() is the better place? */
    const char *out_text = NULL;
    char *filename = NULL;

    if (!data || !state)
        return NULL;

    id3 = get_mp3entry_from_offset(token->next? 1: offset, &filename);
    if (id3)
        filename = id3->path;

#if CONFIG_RTC
    struct tm* tm = NULL;

    /* if the token is an RTC one, update the time
       and do the necessary checks */
    if (token->type >= SKIN_TOKENS_RTC_BEGIN
        && token->type <= SKIN_TOKENS_RTC_END)
    {
        tm = get_time();

        if (!valid_time(tm))
            return NULL;
    }
#endif

    int limit = 1;
    if (intval)
    {
        limit = *intval;
        *intval = -1;
    }

    if (id3 && id3 == state->id3 && id3->cuesheet )
    {
        out_text = get_cuesheetid3_token(token, id3,
                                         token->next?1:offset, buf, buf_size);
        if (out_text)
            return out_text;
    }
    out_text = get_id3_token(token, id3, filename, buf, buf_size, limit, intval);
    if (out_text)
        return out_text;
#if CONFIG_TUNER
    out_text = get_radio_token(token, offset, buf, buf_size, limit, intval);
    if (out_text)
        return out_text;
#endif

    switch (token->type)
    {
        case SKIN_TOKEN_LOGICAL_IF:
        {
            struct logical_if *lif = SKINOFFSETTOPTR(get_skin_buffer(data), token->value.data);
            return get_lif_token_value(gwps, lif, offset, buf, buf_size);
        }
        break;
        case SKIN_TOKEN_LOGICAL_AND:
        case SKIN_TOKEN_LOGICAL_OR:
        {
            int i = 0, truecount = 0;
            char *skinbuffer = get_skin_buffer(data);
            struct skin_element *element =
                    SKINOFFSETTOPTR(skinbuffer, token->value.data);
            if (!element || !element->params) return NULL;
            struct skin_tag_parameter* params =
                    SKINOFFSETTOPTR(skinbuffer, element->params);
            struct skin_tag_parameter* thistag;
            for (i=0; i<element->params_count; i++)
            {
                thistag = &params[i];
                struct skin_element *tokenelement =
                        SKINOFFSETTOPTR(skinbuffer, thistag->data.code);
                if (!tokenelement) return NULL;
                out_text  =  get_token_value(gwps,
                        SKINOFFSETTOPTR(skinbuffer, tokenelement->data),
                        offset, buf, buf_size, intval);
                if (out_text && *out_text)
                    truecount++;
                else if (token->type == SKIN_TOKEN_LOGICAL_AND)
                    return NULL;
            }
            return truecount ? "true" : NULL;
        }
        break;

        case SKIN_TOKEN_SUBSTRING:
        {
            struct substring *ss = SKINOFFSETTOPTR(get_skin_buffer(data), token->value.data);
            if (!ss) return NULL;
            const char *token_val = get_token_value(gwps,
                            SKINOFFSETTOPTR(get_skin_buffer(data), ss->token), offset,
                                                    buf, buf_size, intval);
            if (token_val)
            {
                int start_byte, end_byte, byte_len;
                int utf8_len = utf8length(token_val);

                if (utf8_len < ss->start)
                    return NULL;

                if (ss->start < 0)
                    start_byte = utf8seek(token_val, ss->start + utf8_len);
                else
                    start_byte = utf8seek(token_val, ss->start);

                if (ss->length < 0 || (ss->start + ss->length) > utf8_len)
                    end_byte = strlen(token_val);
                else
                    end_byte = utf8seek(token_val, ss->start + ss->length);

                byte_len = end_byte - start_byte;

                if (token_val != buf)
                    memcpy(buf, &token_val[start_byte], byte_len);
                else
                    buf = &buf[start_byte];

                buf[byte_len] = '\0';
                if (ss->expect_number &&
                    intval && (buf[0] >= '0' && buf[0] <= '9'))
                {

                    numeric_ret = atoi(buf) + 1; /* so 0 is the first item */
                }
                numeric_buf = buf;
                goto gtv_ret_numeric_tag_info;
            }
            return NULL;
        }
        break;

        case SKIN_TOKEN_CHARACTER:
            if (token->value.c == '\n')
                return NULL;
            return &(token->value.c);

        case SKIN_TOKEN_STRING:
            return (char*)SKINOFFSETTOPTR(get_skin_buffer(data), token->value.data);

        case SKIN_TOKEN_TRANSLATEDSTRING:
            return token->value.i < LANG_LAST_INDEX_IN_ARRAY ?
                                   (char*)P2STR(ID2P(token->value.i)) : "<ERR>";

        case SKIN_TOKEN_PLAYLIST_ENTRIES:
            numeric_ret = playlist_amount();
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_LIST_TITLE_TEXT:
            return sb_get_title(gwps->display->screen_type);
        case SKIN_TOKEN_LIST_TITLE_ICON:
            numeric_ret = sb_get_icon(gwps->display->screen_type);
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_LIST_ITEM_TEXT:
        {
            struct listitem *li = (struct listitem *)SKINOFFSETTOPTR(get_skin_buffer(data), token->value.data);
            if (!li) return NULL;
            return skinlist_get_item_text(li->offset, li->wrap, buf, buf_size);
        }
        case SKIN_TOKEN_LIST_ITEM_ROW:
            numeric_ret = skinlist_get_item_row() + 1;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_LIST_ITEM_COLUMN:
            numeric_ret = skinlist_get_item_column() + 1;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_LIST_ITEM_NUMBER:
            numeric_ret = skinlist_get_item_number() + 1;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_LIST_ITEM_IS_SELECTED:
            return skinlist_is_selected_item()?"s":"";
        case SKIN_TOKEN_LIST_ITEM_ICON:
        {
            struct listitem *li = (struct listitem *)SKINOFFSETTOPTR(get_skin_buffer(data), token->value.data);
            if (!li) return NULL;
            int icon = skinlist_get_item_icon(li->offset, li->wrap);
            numeric_ret = icon;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        case SKIN_TOKEN_LIST_NEEDS_SCROLLBAR:
            return skinlist_needs_scrollbar(gwps->display->screen_type) ? "s" : "";
        case SKIN_TOKEN_PLAYLIST_NAME:
            return playlist_name(NULL, buf, buf_size);

        case SKIN_TOKEN_PLAYLIST_POSITION:
            numeric_ret = playlist_get_display_index()+offset;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_PLAYLIST_SHUFFLE:
            if ( global_settings.playlist_shuffle )
                return "s";
            else
                return NULL;
            break;

        case SKIN_TOKEN_VOLUME:
            fmt_size = format_sound_value(buf, buf_size, SOUND_VOLUME,
                                          global_settings.volume);
            /* FIXME: this is a cheap hack to avoid breaking existing themes.
             * The new formatting includes a unit based on the AUDIOHW_SETTING
             * definition -- on all targets, it's defined to be "dB". But the
             * old formatting was just an integer value, and many themes append
             * "dB" manually. So we need to strip the unit to unbreak all those
             * existing themes.
             */
            if(fmt_size >= 3 && !strcmp(&buf[fmt_size - 3], " dB"))
                buf[fmt_size - 3] = 0;

            if (intval)
            {
                int minvol = sound_min(SOUND_VOLUME);
                if (limit == TOKEN_VALUE_ONLY)
                {
                    numeric_ret = global_settings.volume;
                }
                else if (global_settings.volume == minvol)
                {
                    numeric_ret = 1;
                }
                else if (global_settings.volume == 0)
                {
                    numeric_ret = limit - 1;
                }
                else if (global_settings.volume > 0)
                {
                    numeric_ret = limit;
                }
                else
                {
                    numeric_ret = (limit-3) * (global_settings.volume - minvol - 1)
                                / (-1 - minvol) + 2;
                }
            }
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
#ifdef HAVE_ALBUMART
        case SKIN_TOKEN_ALBUMART_FOUND:
            if (SKINOFFSETTOPTR(get_skin_buffer(data), data->albumart))
            {
                int handle = -1;
                handle = playback_current_aa_hid(data->playback_aa_slot);
#if CONFIG_TUNER
                if (in_radio_screen() || (get_radio_status() != FMRADIO_OFF))
                {
                    struct skin_albumart *aa = SKINOFFSETTOPTR(get_skin_buffer(data), data->albumart);
                    if (!aa) return NULL;
                    struct dim dim = {aa->width, aa->height};
                    handle = radio_get_art_hid(&dim);
                }
#endif
                if (handle >= 0)
                    return "C";
            }
            return NULL;
#endif /* def HAVE_ALBUMART */

        case SKIN_TOKEN_BATTERY_PERCENT:
        {
            int l = battery_level();

            if (intval)
            {
                if (limit == TOKEN_VALUE_ONLY)
                {
                    numeric_ret = l;
                }
                else
                {
                    limit = MAX(limit, 3);
                    if (l > -1) {
                        /* First enum is used for "unknown level",
                         * last enum is used for 100%.
                         */
                        numeric_ret = (limit - 2) * l / 100 + 2;
                    } else {
                        numeric_ret = 1;
                    }
                }
            }

            if (l > -1) {
                snprintf(buf, buf_size, "%d", l);
                numeric_buf = buf;
                goto gtv_ret_numeric_tag_info;
            } else {
                numeric_buf = "?";
                goto gtv_ret_numeric_tag_info;
            }
        }

        case SKIN_TOKEN_BATTERY_VOLTS:
        {
            int v = battery_voltage();
            if (v >= 0) {
                snprintf(buf, buf_size, "%d.%02d", v / 1000, (v % 1000) / 10);
                return buf;
            } else {
                return "?";
            }
        }

        case SKIN_TOKEN_BATTERY_TIME:
        {
            int t = battery_time();
            if (t >= 0)
                snprintf(buf, buf_size, "%dh %dm", t / 60, t % 60);
            else
                return "?h ?m";
            return buf;
        }

#if CONFIG_CHARGING
        case SKIN_TOKEN_BATTERY_CHARGER_CONNECTED:
        {
            if(charger_input_state==CHARGER)
                return "p";
            else
                return NULL;
        }
#endif
#if CONFIG_CHARGING >= CHARGING_MONITOR
        case SKIN_TOKEN_BATTERY_CHARGING:
        {
            if (charge_state == CHARGING || charge_state == TOPOFF) {
                return "c";
            } else {
                return NULL;
            }
        }
#endif
        case SKIN_TOKEN_USB_INSERTED:
            if (usb_inserted())
                return "u";
            return NULL;
        case SKIN_TOKEN_BATTERY_SLEEPTIME:
        {
            if (get_sleep_timer() == 0)
                return NULL;
            else
            {
                format_time(buf, buf_size, get_sleep_timer() * 1000);
                return buf;
            }
        }

        case SKIN_TOKEN_PLAYBACK_STATUS:
        {
            int status = current_playmode();
            switch (status) {
            case STATUS_STOP:
                numeric_ret = 1;
                break;
            case STATUS_PLAY:
                numeric_ret = 2;
                break;
            default:
                numeric_ret = status + 1;
                break;
            }

            snprintf(buf, buf_size, "%d", numeric_ret-1);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }

        case SKIN_TOKEN_REPEAT_MODE:
            snprintf(buf, buf_size, "%d", global_settings.repeat_mode);
            numeric_ret = global_settings.repeat_mode + 1;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        case SKIN_TOKEN_RTC_PRESENT:
#if CONFIG_RTC
                return "c";
#else
                return NULL;
#endif

#if CONFIG_RTC
        case SKIN_TOKEN_RTC_12HOUR_CFG:
            snprintf(buf, buf_size, "%d", global_settings.timeformat);
            numeric_ret = global_settings.timeformat + 1;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_DAY_OF_MONTH:
            /* d: day of month (01..31) */
            snprintf(buf, buf_size, "%02d", tm->tm_mday);
            numeric_ret = tm->tm_mday - 1;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_DAY_OF_MONTH_BLANK_PADDED:
            /* e: day of month, blank padded ( 1..31) */
            snprintf(buf, buf_size, "%2d", tm->tm_mday);
            numeric_ret = tm->tm_mday - 1;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_HOUR_24_ZERO_PADDED:
            /* H: hour (00..23) */
            numeric_ret = tm->tm_hour;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_HOUR_24:
            /* k: hour ( 0..23) */
            numeric_ret = tm->tm_hour;
            snprintf(buf, buf_size, "%2d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_HOUR_12_ZERO_PADDED:
            /* I: hour (01..12) */
            numeric_ret = (tm->tm_hour % 12 == 0) ? 12 : tm->tm_hour % 12;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_HOUR_12:
            /* l: hour ( 1..12) */
            numeric_ret = (tm->tm_hour % 12 == 0) ? 12 : tm->tm_hour % 12;
            snprintf(buf, buf_size, "%2d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_MONTH:
            /* m: month (01..12) */
            numeric_ret = tm->tm_mon + 1;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_MINUTE:
            /* M: minute (00..59) */
            numeric_ret = tm->tm_min;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_SECOND:
            /* S: second (00..59) */
            numeric_ret = tm->tm_sec;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_YEAR_2_DIGITS:
            /* y: last two digits of year (00..99) */
            numeric_ret = tm->tm_year % 100;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_YEAR_4_DIGITS:
            /* Y: year (1970...) */
            numeric_ret = tm->tm_year + 1900;
            snprintf(buf, buf_size, "%04d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_AM_PM_UPPER:
            /* p: upper case AM or PM indicator */
            numeric_ret = tm->tm_hour/12 == 0 ? 0 : 1;
            numeric_buf = numeric_ret == 0 ? "AM" : "PM";
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_AM_PM_LOWER:
            /* P: lower case am or pm indicator */
            numeric_ret= tm->tm_hour/12 == 0 ? 0 : 1;
            numeric_buf = numeric_ret == 0 ? "am" : "pm";
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_WEEKDAY_NAME:
            /* a: abbreviated weekday name (Sun..Sat) */
            return str(LANG_WEEKDAY_SUNDAY + tm->tm_wday);

        case SKIN_TOKEN_RTC_MONTH_NAME:
            /* b: abbreviated month name (Jan..Dec) */
            return str(LANG_MONTH_JANUARY + tm->tm_mon);

        case SKIN_TOKEN_RTC_DAY_OF_WEEK_START_MON:
            /* u: day of week (1..7); 1 is Monday */
            snprintf(buf, buf_size, "%1d", tm->tm_wday + 1);
            numeric_ret = (tm->tm_wday == 0) ? 7 : tm->tm_wday;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_RTC_DAY_OF_WEEK_START_SUN:
            /* w: day of week (0..6); 0 is Sunday */
            snprintf(buf, buf_size, "%1d", tm->tm_wday);
            numeric_ret = tm->tm_wday + 1;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
#else   /* !CONFIG_RTC */
        case SKIN_TOKEN_RTC_DAY_OF_MONTH:
        case SKIN_TOKEN_RTC_DAY_OF_MONTH_BLANK_PADDED:
        case SKIN_TOKEN_RTC_HOUR_24_ZERO_PADDED:
        case SKIN_TOKEN_RTC_HOUR_24:
        case SKIN_TOKEN_RTC_HOUR_12_ZERO_PADDED:
        case SKIN_TOKEN_RTC_HOUR_12:
        case SKIN_TOKEN_RTC_MONTH:
        case SKIN_TOKEN_RTC_MINUTE:
        case SKIN_TOKEN_RTC_SECOND:
        case SKIN_TOKEN_RTC_AM_PM_UPPER:
        case SKIN_TOKEN_RTC_AM_PM_LOWER:
        case SKIN_TOKEN_RTC_YEAR_2_DIGITS:
            return "--";
        case SKIN_TOKEN_RTC_YEAR_4_DIGITS:
            return "----";
        case SKIN_TOKEN_RTC_WEEKDAY_NAME:
        case SKIN_TOKEN_RTC_MONTH_NAME:
            return "---";
        case SKIN_TOKEN_RTC_DAY_OF_WEEK_START_MON:
        case SKIN_TOKEN_RTC_DAY_OF_WEEK_START_SUN:
            return "-";
#endif /* CONFIG_RTC */

        /* peakmeter */
        case SKIN_TOKEN_PEAKMETER_LEFT:
        case SKIN_TOKEN_PEAKMETER_RIGHT:
        {
            int left, right, val;
            peak_meter_current_vals(&left, &right);
            val = token->type == SKIN_TOKEN_PEAKMETER_LEFT ?
                    left : right;
            val = peak_meter_scale_value(val, limit==1 ? MAX_PEAK : limit);
            numeric_ret = val;
            snprintf(buf, buf_size, "%d", numeric_ret);
            data->peak_meter_enabled = true;
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }

        case SKIN_TOKEN_CROSSFADE:
#ifdef HAVE_CROSSFADE
            snprintf(buf, buf_size, "%d", global_settings.crossfade);
            numeric_ret = global_settings.crossfade + 1;
#else
            snprintf(buf, buf_size, "%d", 0);
#endif
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;

        case SKIN_TOKEN_REPLAYGAIN:
        {
            int globtype = global_settings.replaygain_settings.type;
            int val;


            if (globtype == REPLAYGAIN_OFF)
                val = 1; /* off */
            else
            {
                int type = id3_get_replaygain_mode(id3);

                if (type < 0)
                    val = 6;    /* no tag */
                else
                    val = type + 2;

                if (globtype == REPLAYGAIN_SHUFFLE)
                    val += 2;
            }

            numeric_ret = val;
            switch (val)
            {
                case 1:
                case 6:
                    numeric_buf = "+0.00 dB";;
                    goto gtv_ret_numeric_tag_info;
                /* due to above, coming here with !id3 shouldn't be possible */
                case 2:
                case 4:
                    replaygain_itoa(buf, buf_size, id3->track_level);
                    break;
                case 3:
                case 5:
                    replaygain_itoa(buf, buf_size, id3->album_level);
                    break;
            }
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }

#if defined (HAVE_PITCHCONTROL)
        case SKIN_TOKEN_SOUND_PITCH:
        {
            int32_t pitch = sound_get_pitch();
            snprintf(buf, buf_size, "%ld.%ld",
                     pitch / PITCH_SPEED_PRECISION,
             (pitch  % PITCH_SPEED_PRECISION) / (PITCH_SPEED_PRECISION / 10));
            if (intval)
                numeric_ret = pitch_speed_enum(limit, pitch, PITCH_SPEED_PRECISION * 100);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
#endif

#if defined (HAVE_PITCHCONTROL)
    case SKIN_TOKEN_SOUND_SPEED:
    {
        int32_t pitch = sound_get_pitch();
        int32_t speed;
        if (dsp_timestretch_available())
            speed = GET_SPEED(pitch, dsp_get_timestretch());
        else
            speed = pitch;
        snprintf(buf, buf_size, "%ld.%ld",
                     speed / PITCH_SPEED_PRECISION,
             (speed  % PITCH_SPEED_PRECISION) / (PITCH_SPEED_PRECISION / 10));
        if (intval)
            numeric_ret = pitch_speed_enum(limit, speed, PITCH_SPEED_PRECISION * 100);
        numeric_buf = buf;
        goto gtv_ret_numeric_tag_info;
    }
#endif

        case SKIN_TOKEN_MAIN_HOLD:
#ifdef HAVE_TOUCHSCREEN
            if (data->touchscreen_locked)
                return "t";
#endif
#ifdef HAS_BUTTON_HOLD
            if (button_hold())
#else
            if (is_keys_locked())
#endif /*hold switch or softlock*/
                return "h";
            else
                return NULL;

#ifdef HAS_REMOTE_BUTTON_HOLD
        case SKIN_TOKEN_REMOTE_HOLD:
            if (remote_button_hold())
                return "r";
            else
                return NULL;
#endif

#if (CONFIG_LED == LED_VIRTUAL) || defined(HAVE_REMOTE_LCD)
        case SKIN_TOKEN_VLED_HDD:
            if(led_read(HZ/2))
                return "h";
            else
                return NULL;
#endif
        case SKIN_TOKEN_BUTTON_VOLUME:
            if (global_status.last_volume_change &&
                TIME_BEFORE(current_tick, global_status.last_volume_change +
                                          token->value.i))
                return "v";
            return NULL;
        case SKIN_TOKEN_LASTTOUCH:
            {
#ifdef HAVE_TOUCHSCREEN
            long last_touch = touchscreen_last_touch();
            char *skin_base = get_skin_buffer(data);
            struct touchregion_lastpress *data = SKINOFFSETTOPTR(skin_base, token->value.data);
            if (!data) return NULL;
            struct touchregion *region = SKINOFFSETTOPTR(skin_base, data->region);
            if (region)
                last_touch = region->last_press;

            if (last_touch != -1 &&
                TIME_BEFORE(current_tick, data->timeout + last_touch))
                return "t";
#endif
            }
            return NULL;
        case SKIN_TOKEN_HAVE_TOUCH:
#ifdef HAVE_TOUCHSCREEN
            return "t";
#else
            return NULL;
#endif

        case SKIN_TOKEN_SETTING:
        {
            const struct settings_list *s = settings+token->value.i;
            if (intval)
            {
                /* Handle contionals */
                switch (s->flags&F_T_MASK)
                {
                    case F_T_INT:
                    case F_T_UINT:
                        if (s->flags&F_T_SOUND)
                        {
                            /* %?St|name|<min|min+1|...|max-1|max> */
                            int sound_setting = s->sound_setting->setting;
                            /* settings with decimals can't be used in conditionals */
                            if (sound_numdecimals(sound_setting) == 0)
                            {
                                numeric_ret = (*(int*)s->setting-sound_min(sound_setting))
                                      /sound_steps(sound_setting) + 1;
                            }
                            else
                                numeric_ret = -1;
                        }
                        else if (s->flags&F_RGB)
                            /* %?St|name|<#000000|#000001|...|#FFFFFF> */
                            /* shouldn't overflow since colors are stored
                             * on 16 bits ...
                             * but this is pretty useless anyway */
                            numeric_ret = *(int*)s->setting + 1;
                        else if (s->cfg_vals == NULL)
                            /* %?St|name|<1st choice|2nd choice|...> */
                            numeric_ret = (*(int*)s->setting-s->int_setting->min)
                                      /s->int_setting->step + 1;
                        else
                            /* %?St|name|<1st choice|2nd choice|...> */
                            /* Not sure about this one. cfg_name/vals are
                             * indexed from 0 right? */
                            numeric_ret = *(int*)s->setting + 1;
                        break;
                    case F_T_BOOL:
                        /* %?St|name|<if true|if false> */
                        numeric_ret = *(bool*)s->setting?1:2;
                        break;
                    case F_T_CHARPTR:
                    case F_T_UCHARPTR:
                        /* %?St|name|<if non empty string|if empty>
                         * The string's emptyness discards the setting's
                         * prefix and suffix */
                        numeric_ret = ((char*)s->setting)[0]?1:2;
                        /* if there is a prefix we should ignore it here */
                        if (s->filename_setting->prefix)
                        {
                            numeric_buf = (char*)s->setting;
                            goto gtv_ret_numeric_tag_info;
                        }
                        break;
                    default:
                        /* This shouldn't happen ... but you never know */
                        numeric_ret = -1;
                        break;
                }
            }

            /* Special handlng for filenames because we dont want to show the prefix */
            if ((s->flags&F_T_MASK) == F_T_CHARPTR ||
                (s->flags&F_T_MASK) == F_T_UCHARPTR)
            {
                if (s->filename_setting->prefix)
                {
                    numeric_buf = (char*)s->setting;
                    goto gtv_ret_numeric_tag_info;
                }
            }
            cfg_to_string(token->value.i,buf,buf_size);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        case SKIN_TOKEN_HAVE_TUNER:
#if CONFIG_TUNER
            if (radio_hardware_present())
                return "r";
#endif
            return NULL;
        /* Recording tokens */
        case SKIN_TOKEN_HAVE_RECORDING:
#ifdef HAVE_RECORDING
            return "r";
#else
            return NULL;
#endif

#ifdef HAVE_RECORDING
        case SKIN_TOKEN_IS_RECORDING:
            if (audio_status() == AUDIO_STATUS_RECORD)
                return "r";
            return NULL;
        case SKIN_TOKEN_REC_FREQ: /* order from REC_FREQ_CFG_VAL_LIST */
        {
            unsigned long samprk;
            int rec_freq = global_settings.rec_frequency;

#ifdef SIMULATOR
            samprk = 44100;
#else
#if defined(HAVE_SPDIF_REC)
            if (global_settings.rec_source == AUDIO_SRC_SPDIF)
            {
                /* Use rate in use, not current measured rate if it changed */
                samprk = pcm_rec_sample_rate();
                rec_freq = 0;
                while (rec_freq < SAMPR_NUM_FREQ &&
                       audio_master_sampr_list[rec_freq] != samprk)
                {
                    rec_freq++;
                }
            }
            else
#endif
                samprk = rec_freq_sampr[rec_freq];
#endif /* SIMULATOR */
            switch (rec_freq)
            {
                REC_HAVE_96_(case REC_FREQ_96:
                    numeric_ret = 1;
                    break;)
                REC_HAVE_88_(case REC_FREQ_88:
                    numeric_ret = 2;
                    break;)
                REC_HAVE_64_(case REC_FREQ_64:
                    numeric_ret = 3;
                    break;)
                REC_HAVE_48_(case REC_FREQ_48:
                    numeric_ret = 4;
                    break;)
                REC_HAVE_44_(case REC_FREQ_44:
                    numeric_ret = 5;
                    break;)
                REC_HAVE_32_(case REC_FREQ_32:
                    numeric_ret = 6;
                    break;)
                REC_HAVE_24_(case REC_FREQ_24:
                    numeric_ret = 7;
                    break;)
                REC_HAVE_22_(case REC_FREQ_22:
                    numeric_ret = 8;
                    break;)
                REC_HAVE_16_(case REC_FREQ_16:
                    numeric_ret = 9;
                    break;)
                REC_HAVE_12_(case REC_FREQ_12:
                    numeric_ret = 10;
                    break;)
                REC_HAVE_11_(case REC_FREQ_11:
                    numeric_ret = 11;
                    break;)
                REC_HAVE_8_(case REC_FREQ_8:
                    numeric_ret = 12;
                    break;)
            }
            snprintf(buf, buf_size, "%lu.%1lu", samprk/1000,samprk%1000);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        case SKIN_TOKEN_REC_ENCODER:
        {
            int rec_format = global_settings.rec_format+1; /* WAV, AIFF, WV, MPEG */
            if (intval)
                *intval = rec_format;
            switch (rec_format)
            {
                case REC_FORMAT_PCM_WAV:
                    return "wav";
                case REC_FORMAT_AIFF:
                    return "aiff";
                case REC_FORMAT_WAVPACK:
                    return "wv";
                case REC_FORMAT_MPA_L3:
                    return "MP3";
                default:
                    return NULL;
            }
            break;
        }
        case SKIN_TOKEN_REC_BITRATE:
            if (global_settings.rec_format == REC_FORMAT_MPA_L3)
            {
                #if 0 /* FIXME: I dont know if this is needed? */
                switch (1<<global_settings.mp3_enc_config.bitrate)
                {
                    case MP3_BITR_CAP_8:
                        numeric_ret = 1;
                        break;
                    case MP3_BITR_CAP_16:
                        numeric_ret = 2;
                        break;
                    case MP3_BITR_CAP_24:
                        numeric_ret = 3;
                        break;
                    case MP3_BITR_CAP_32:
                        numeric_ret = 4;
                        break;
                    case MP3_BITR_CAP_40:
                        numeric_ret = 5;
                        break;
                    case MP3_BITR_CAP_48:
                        numeric_ret = 6;
                        break;
                    case MP3_BITR_CAP_56:
                        numeric_ret = 7;
                        break;
                    case MP3_BITR_CAP_64:
                        numeric_ret = 8;
                        break;
                    case MP3_BITR_CAP_80:
                        numeric_ret = 9;
                        break;
                    case MP3_BITR_CAP_96:
                        numeric_ret = 10;
                        break;
                    case MP3_BITR_CAP_112:
                        numeric_ret = 11;
                        break;
                    case MP3_BITR_CAP_128:
                        numeric_ret = 12;
                        break;
                    case MP3_BITR_CAP_144:
                        numeric_ret = 13;
                        break;
                    case MP3_BITR_CAP_160:
                        numeric_ret = 14;
                        break;
                    case MP3_BITR_CAP_192:
                        numeric_ret = 15;
                        break;
                }
                #endif
                numeric_ret = global_settings.mp3_enc_config.bitrate+1;
                snprintf(buf, buf_size, "%lu", global_settings.mp3_enc_config.bitrate+1);
                numeric_buf = buf;
                goto gtv_ret_numeric_tag_info;
            }
            else
                return NULL; /* Fixme later */
        case SKIN_TOKEN_REC_MONO:
            if (!global_settings.rec_channels)
                return "m";
            return NULL;

        case SKIN_TOKEN_REC_SECONDS:
        {
            int time = (audio_recorded_time() / HZ) % 60;
            numeric_ret = time;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        case SKIN_TOKEN_REC_MINUTES:
        {
            int time = (audio_recorded_time() / HZ) / 60;
            numeric_ret = time;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        case SKIN_TOKEN_REC_HOURS:
        {
            int time = (audio_recorded_time() / HZ) / 3600;
            numeric_ret = time;
            snprintf(buf, buf_size, "%02d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }

#endif /* HAVE_RECORDING */

        case SKIN_TOKEN_CURRENT_SCREEN:
        {
            int curr_screen = get_current_activity();
            numeric_ret = curr_screen;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }

        case SKIN_TOKEN_LANG_IS_RTL:
            return lang_is_rtl() ? "r" : NULL;

#ifdef HAVE_SKIN_VARIABLES
        case SKIN_TOKEN_VAR_GETVAL:
        {
            char *skin_base = get_skin_buffer(data);
            struct skin_var* var = SKINOFFSETTOPTR(skin_base, token->value.data);
            numeric_ret = var->value;
            snprintf(buf, buf_size, "%d", numeric_ret);
            numeric_buf = buf;
            goto gtv_ret_numeric_tag_info;
        }
        break;
        case SKIN_TOKEN_VAR_TIMEOUT:
            {
            char *skin_base = get_skin_buffer(data);
            struct skin_var_lastchange *data = SKINOFFSETTOPTR(skin_base, token->value.data);
            struct skin_var* var = SKINOFFSETTOPTR(skin_base, data->var);
            unsigned int last_change = var->last_changed;

            if (last_change != 0xffff &&
                TIME_BEFORE(current_tick, data->timeout + last_change))
                return "t";
            }
            return NULL;
#endif
        default:
            return NULL;
    }

gtv_ret_numeric_tag_info:
    if (intval)
    {
        *intval = numeric_ret;
    }
    return numeric_buf;
}
