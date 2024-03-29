/*
 * Copyright (C) 2008 by Openmoko, Inc.
 * Written by Paul-Valentin Borza <paul@borza.ro>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#include "accelneo.h"

static unsigned char go_on;

/*
 * opens one accelerometer and the touchscreen
 */
unsigned char neo_open(struct neo_t *neo, enum neo_accel w_accel)
{
	/* open one of the two accelerometers (top or bottom) */
	if (w_accel == neo_accel2) {
		neo->accel_desc = open("/dev/input/event2", O_RDONLY);
	} else if (w_accel == neo_accel3) {
		neo->accel_desc = open("/dev/input/event3", O_RDONLY);
	} else {
		neo->accel_desc = -1;
	}
	
	/* use screen (event1) */
	neo->screen_desc = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
	
	if ((neo->accel_desc < 0) || (neo->screen_desc < 0)) {
		return 0;
	} else {
		go_on = 1;
		return 1;
	}
}

/*
 * closes the accelerometer and the touchscreen
 */
void neo_close(struct neo_t *neo)
{
	go_on = 0;
	close(neo->accel_desc);
	close(neo->screen_desc);
}

/*
 * begins reading the accelerometer and the touchscreen
 */
void neo_begin_read(struct neo_t *neo)
{
	const unsigned int report_len = 16;
	unsigned char report[report_len];
	unsigned char pressed = 0;
	struct accel_3d_t accel;
	accel.val[0] = 0.0;
	accel.val[1] = 0.0;
	accel.val[2] = 0.0;
	
	while (go_on)
	{
		int read_len = read(neo->accel_desc, report, report_len);
		/* this was a blocking read */
		if (read_len < 0)
		{
			perror("read");
			continue;
		}
		
		unsigned short int rel = *(unsigned short int *)(report + 8);
		/* 
		 * Neo sends three reports on X, Y, and Z with rel = 2
		 * and another one (as a separator) with rel = 0 
		 */
		if (rel == 2)
		{
			unsigned short int axis_ind = *(short int *)(report + 10);
			/* receives signed acceleration in milli-G */
			int val_mg = *(int *)(report + 12);
			/* convert acceleration to G */
			float val_g = (float)val_mg / 1000.0;
			
			/* save to accel on the axis */
			accel.val[axis_ind] = val_g;
		}
		else if (rel == 0) /* separator report */
		{
			/* is touchscreen pressed? */
			int read_len = read(neo->screen_desc, report, report_len);
			/* this was a non-blocking read */
			//if (read_len < 0)
			//{
			//	perror("read");
			//	continue;
			//}
			pressed = (read_len > 0);
			/* call back is called */
			neo->handle_recv(pressed, accel);
		}
	}
}

