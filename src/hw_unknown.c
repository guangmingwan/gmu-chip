/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2011 Johannes Heimansberg (wejp.k.vu)
 *
 * File: hw_unknown.c  Created: 090629
 *
 * Description: Hardware specific header file for unknown devices (such as PCs)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include <stdio.h>
#include "debug.h"
#include "hw_unknown.h"

static int selected_mixer = -1;

void hw_display_off(void)
{
	wdprintf(V_DEBUG, "hw_unknown", "Display off requested.\n");
}

void hw_display_on(void)
{
	wdprintf(V_DEBUG, "hw_unknown", "Display on requested.\n");
}

int hw_open_mixer(int mixer_channel)
{
}
void hw_close_mixer(void)
{
}

void hw_set_volume(int volume)
{
}

void hw_detect_device_model(void)
{
}

const char *hw_get_device_model_name(void)
{
	return "Unknown device";
}
