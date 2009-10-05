/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2008 by Jonathan Gordon
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

#include <stdlib.h>
#include "config.h"
#include "lcd.h"
#include "lcd-remote.h"
#include "font.h"
#include "viewport.h"
#include "screen_access.h"
#include "settings.h"
#include "misc.h"

/*some short cuts for fg/bg/line selector handling */
#ifdef HAVE_LCD_COLOR
#define LINE_SEL_FROM_SETTINGS(vp) \
    do { \
        vp->lss_pattern = global_settings.lss_color; \
        vp->lse_pattern = global_settings.lse_color; \
        vp->lst_pattern = global_settings.lst_color; \
    } while (0)
#define FG_FALLBACK global_settings.fg_color
#define BG_FALLBACK global_settings.bg_color
#else
/* mono/greyscale doesn't have most of the above */
#define LINE_SEL_FROM_SETTINGS(vp)
#define FG_FALLBACK LCD_DEFAULT_FG
#define BG_FALLBACK LCD_DEFAULT_BG
#endif

/* all below isn't needed for pc tools (i.e. checkwps/wps editor)
 * only viewport_parse_viewport() is */
#ifndef __PCTOOL__
#include "sprintf.h"
#include "string.h"
#include "kernel.h"
#include "system.h"
#include "statusbar.h"
#include "appevents.h"
#ifdef HAVE_LCD_BITMAP
#include "language.h"
#endif

static int statusbar_enabled = 0;

#ifdef HAVE_LCD_BITMAP

static struct {
    struct  viewport* vp;
    int     active[NB_SCREENS];
} ui_vp_info;

static struct viewport custom_vp[NB_SCREENS];

/* callbacks for GUI_EVENT_* events */
static void viewportmanager_ui_vp_changed(void *param);
static void statusbar_toggled(void* param);
static unsigned viewport_init_ui_vp(void);
#endif
static void viewportmanager_redraw(void* data);

int viewport_get_nb_lines(struct viewport *vp)
{
#ifdef HAVE_LCD_BITMAP
    return vp->height/font_get(vp->font)->height;
#else
    (void)vp;
    return 2;
#endif
}

static bool showing_bars(enum screen_type screen)
{
    if (statusbar_enabled & VP_SB_ONSCREEN(screen))
    {
#ifdef HAVE_LCD_BITMAP
        bool ignore = statusbar_enabled & VP_SB_IGNORE_SETTING(screen);
        return ignore || (statusbar_position(screen));
#else
        return true;
#endif
    }
    return false;
}

void viewport_set_fullscreen(struct viewport *vp, enum screen_type screen)
{
    vp->x = 0;
    vp->width = screens[screen].lcdwidth;

#ifdef HAVE_LCD_BITMAP
    vp->drawmode = DRMODE_SOLID;
    vp->font = FONT_UI; /* default to UI to discourage SYSFONT use */
        
    vp->height = screens[screen].lcdheight;
    if (statusbar_position(screen) != STATUSBAR_BOTTOM && showing_bars(screen))
        vp->y = STATUSBAR_HEIGHT;
    else 
        vp->y = 0;
#else
    vp->y = 0;
#endif
    vp->height = screens[screen].lcdheight - (showing_bars(screen)?STATUSBAR_HEIGHT:0);

#if LCD_DEPTH > 1
#ifdef HAVE_REMOTE_LCD
    /* We only need this test if there is a remote LCD */
    if (screen == SCREEN_MAIN)
#endif
    {
        vp->fg_pattern = FG_FALLBACK;
        vp->bg_pattern = BG_FALLBACK;
        LINE_SEL_FROM_SETTINGS(vp);
    }
#endif

#if defined(HAVE_REMOTE_LCD) && LCD_REMOTE_DEPTH > 1
    if (screen == SCREEN_REMOTE)
    {
        vp->fg_pattern = LCD_REMOTE_DEFAULT_FG;
        vp->bg_pattern = LCD_REMOTE_DEFAULT_BG;
    }
#endif
}

void viewport_set_defaults(struct viewport *vp, enum screen_type screen)
{
#ifdef HAVE_LCD_BITMAP
    if (ui_vp_info.active[screen])
        *vp = ui_vp_info.vp[screen];
    else
#endif
        viewport_set_fullscreen(vp, screen);

#ifdef HAVE_LCD_BITMAP
    vp->flags &= ~VP_IS_RTL;
    vp->flags |= lang_is_rtl() ? VP_IS_RTL : 0;
#endif
}

void viewportmanager_init(void)
{
#ifdef HAVE_LCD_BITMAP
    int retval, i;
    add_event(GUI_EVENT_STATUSBAR_TOGGLE, false, statusbar_toggled);
    retval = viewport_init_ui_vp();
    FOR_NB_SCREENS(i)
        ui_vp_info.active[i] = retval & BIT_N(i);
    ui_vp_info.vp = custom_vp;
#endif
    viewportmanager_set_statusbar(VP_SB_ALLSCREENS);
}

int viewportmanager_get_statusbar(void)
{
    return statusbar_enabled;
}

int viewportmanager_set_statusbar(int enabled)
{
    int old = statusbar_enabled;
    statusbar_enabled = enabled;
    if (enabled)
    {
        int i;
        FOR_NB_SCREENS(i)
        {
            if (showing_bars(i))
                gui_statusbar_draw(&statusbars.statusbars[i], true);
        }
        add_event(GUI_EVENT_ACTIONUPDATE, false, viewportmanager_redraw);
    }
    else
    {
        remove_event(GUI_EVENT_ACTIONUPDATE, viewportmanager_redraw);
    }
    return old;
}

static void viewportmanager_redraw(void* data)
{
    int i;

    FOR_NB_SCREENS(i)
    {
        if (showing_bars(i))
            gui_statusbar_draw(&statusbars.statusbars[i], NULL != data);
    }
}
#ifdef HAVE_LCD_BITMAP

static void statusbar_toggled(void* param)
{
    (void)param;
    /* update vp manager for the new setting and reposition vps
     * if necessary */
    viewportmanager_theme_changed(THEME_STATUSBAR);
}

void viewportmanager_theme_changed(int which)
{
    int i;
#ifdef HAVE_BUTTONBAR
    if (which & THEME_BUTTONBAR)
    {   /* don't handle further, the custom ui viewport ignores the buttonbar,
         * as does viewport_set_defaults(), since only lists use it*/
        screens[SCREEN_MAIN].has_buttonbar = global_settings.buttonbar;
    }
#endif
    if (which & THEME_UI_VIEWPORT)
    {
        int retval = viewport_init_ui_vp();
        /* reset the ui viewport */
        FOR_NB_SCREENS(i)
            ui_vp_info.active[i] = retval & BIT_N(i);

        if (retval != 0)
            add_event(GUI_EVENT_REFRESH, false, viewportmanager_ui_vp_changed);
        else
            remove_event(GUI_EVENT_REFRESH, viewportmanager_ui_vp_changed);
        /* and point to it */
        ui_vp_info.vp = custom_vp;
    }
    if (which & THEME_STATUSBAR)
    {
        statusbar_enabled = VP_SB_HIDE_ALL;

        FOR_NB_SCREENS(i)
        {
            if (statusbar_position(i) != STATUSBAR_OFF)
                statusbar_enabled  |= VP_SB_ONSCREEN(i);
        }

        if (statusbar_enabled != VP_SB_HIDE_ALL)
            add_event(GUI_EVENT_ACTIONUPDATE, false, viewportmanager_redraw);
        else
            remove_event(GUI_EVENT_ACTIONUPDATE, viewportmanager_redraw);

        /* reposition viewport to fit statusbar, only if not using the ui vp */
        
        FOR_NB_SCREENS(i)
        {
            if (!ui_vp_info.active[i])
                viewport_set_fullscreen(&custom_vp[i], i);
        }
    }
    send_event(GUI_EVENT_THEME_CHANGED, NULL);
}

static void viewportmanager_ui_vp_changed(void *param)
{
    /* if the user changed the theme, we need to initiate a full redraw */
    int i;
    /* cast param to a function */
    void (*draw_func)(void) = ((void(*)(void))param);
    /* start with clearing the screen */
    FOR_NB_SCREENS(i)
        screens[i].clear_display();
    /* redraw the statusbar if it was enabled */
    viewportmanager_set_statusbar(statusbar_enabled);
    /* call the passed function which will redraw the content of
     * the current screen */
    if (param != NULL)
        draw_func();
    FOR_NB_SCREENS(i)
        screens[i].update();
}

void viewport_set_current_vp(struct viewport* vp)
{
    if (vp != NULL)
        ui_vp_info.vp = vp;
    else
        ui_vp_info.vp = custom_vp;

    /* must be done after the assignment above or event handler get old vps */
    send_event(GUI_EVENT_THEME_CHANGED, NULL);
}

struct viewport* viewport_get_current_vp(void)
{
    return ui_vp_info.vp;
}

bool viewport_ui_vp_get_state(enum screen_type screen)
{
    return ui_vp_info.active[screen];
}

/*
 * (re)parse the UI vp from the settings
 *  - Returns
 *          0 if no UI vp is used at all
 *          else the bit for the screen (1<<screen) is set
 */
static unsigned viewport_init_ui_vp(void)
{
    int screen;
    unsigned ret = 0;
    char *setting;
    FOR_NB_SCREENS(screen)
    {
#ifdef HAVE_REMOTE_LCD
        if ((screen == SCREEN_REMOTE))
            setting = global_settings.remote_ui_vp_config;
        else
#endif
            setting = global_settings.ui_vp_config;

            
        if (!(viewport_parse_viewport(&custom_vp[screen], screen,
                 setting, ',')))
            viewport_set_fullscreen(&custom_vp[screen], screen);
        else
            ret |= BIT_N(screen);
    }
    return ret;
}

#ifdef HAVE_TOUCHSCREEN
/* check if a point (x and y coordinates) are within a viewport */
bool viewport_point_within_vp(const struct viewport *vp, int x, int y)
{
    bool is_x = (x >= vp->x && x < (vp->x + vp->width));
    bool is_y = (y >= vp->y && y < (vp->y + vp->height));
    return (is_x && is_y);
}
#endif /* HAVE_TOUCHSCREEN */
#endif /* HAVE_LCD_BITMAP */
#endif /* __PCTOOL__ */

#ifdef HAVE_LCD_COLOR
#define ARG_STRING(_depth) ((_depth) == 2 ? "dddddgg":"dddddcc")
#else
#define ARG_STRING(_depth) "dddddgg"
#endif

#ifdef HAVE_LCD_BITMAP
const char* viewport_parse_viewport(struct viewport *vp,
                                    enum screen_type screen,
                                    const char *bufptr,
                                    const char separator)
{
    /* parse the list to the viewport struct */
    const char *ptr = bufptr;
    int depth;
    uint32_t set = 0;

    enum {
        PL_X = 0,
        PL_Y,
        PL_WIDTH,
        PL_HEIGHT,
        PL_FONT,
        PL_FG,
        PL_BG,
    };
    
    /* Work out the depth of this display */
    depth = screens[screen].depth;
#if (LCD_DEPTH == 1) || (defined(HAVE_REMOTE_LCD) && LCD_REMOTE_DEPTH == 1)
    if (depth == 1)
    {
        if (!(ptr = parse_list("ddddd", &set, separator, ptr,
                    &vp->x, &vp->y, &vp->width, &vp->height, &vp->font)))
            return NULL;
    }
    else
#endif
#if (LCD_DEPTH > 1) || (defined(HAVE_REMOTE_LCD) && LCD_REMOTE_DEPTH > 1)
    if (depth >= 2)
    {
        if (!(ptr = parse_list(ARG_STRING(depth), &set, separator, ptr,
                    &vp->x, &vp->y, &vp->width, &vp->height, &vp->font,
                    &vp->fg_pattern,&vp->bg_pattern)))
            return NULL;
    }
    else
#endif
    {}
#undef ARG_STRING

    /* X and Y *must* be set */
    if (!LIST_VALUE_PARSED(set, PL_X) || !LIST_VALUE_PARSED(set, PL_Y))
        return NULL;
    
    /* fix defaults */
    if (!LIST_VALUE_PARSED(set, PL_WIDTH))
        vp->width = screens[screen].lcdwidth - vp->x;
    if (!LIST_VALUE_PARSED(set, PL_HEIGHT))
        vp->height = screens[screen].lcdheight - vp->y;

#if (LCD_DEPTH > 1) || (defined(HAVE_REMOTE_LCD) && LCD_REMOTE_DEPTH > 1)
    if (!LIST_VALUE_PARSED(set, PL_FG))
        vp->fg_pattern = FG_FALLBACK;
    if (!LIST_VALUE_PARSED(set, PL_BG))
        vp->bg_pattern = BG_FALLBACK;
#endif /* LCD_DEPTH > 1 || LCD_REMOTE_DEPTH > 1 */

    LINE_SEL_FROM_SETTINGS(vp);

    /* Validate the viewport dimensions - we know that the numbers are
       non-negative integers, ignore bars and assume the viewport takes them
       * into account */
    if ((vp->x >= screens[screen].lcdwidth) ||
        ((vp->x + vp->width) > screens[screen].lcdwidth) ||
        (vp->y >= screens[screen].lcdheight) ||
        ((vp->y + vp->height) > screens[screen].lcdheight))
    {
        return NULL;
    }

    /* Default to using the user font if the font was an invalid number or '-'*/
    if (((vp->font != FONT_SYSFIXED) && (vp->font != FONT_UI))
            || !LIST_VALUE_PARSED(set, PL_FONT)
            )
        vp->font = FONT_UI;

    /* Set the defaults for fields not user-specified */
    vp->drawmode = DRMODE_SOLID;

    return ptr;
}
#endif
