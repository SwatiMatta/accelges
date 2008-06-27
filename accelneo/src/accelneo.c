/*
 * Copyright (C) 2008 by OpenMoko, Inc.
 * Written by Paul-Valentin Borza <gestures@borza.ro>
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
#include <unistd.h>

#include "accelneo.h"

unsigned char neo_open(struct neo_t *neo)
{
	/* use top accelerometer (event2) */
	/* use bottom accelerometer (event3) */
	neo->in_desc = open("/dev/input/event2", O_RDONLY);
	if (neo->in_desc < 0)
	{	return 0;	}
	else
	{	return 1;	}
}

void neo_close(struct neo_t *neo)
{
	close(neo->in_desc);
}

void neo_begin_read(struct neo_t *neo)
{
	const unsigned int report_len = 50;
	unsigned char report[report_len];
	while (1)
	{
		int read_len = read(neo->in_desc, report, report_len);
		if (read_len < 0)
		{
			perror("read");
			continue;
		}
		printf("read %d bytes\n", read_len);
	}
}
