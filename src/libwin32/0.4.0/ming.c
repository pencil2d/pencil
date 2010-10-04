/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* $Id: ming.c,v 1.27 2007/08/27 13:04:16 krechert Exp $ */

#include "ming.h"
#include "libming.h"
#include "blocks/character.h"
#include "font_util.h"
#include "shape_cubic.h"
#include "blocks/error.h"
#include "ming_config.h"


int SWF_versionNum = 5;
int SWF_compression = -1;

float Ming_scale = 20.0;
int Ming_cubicThreshold = 10000;


/*
 * module-wide initialization.
 * returns non-zero if error.
 */

int Ming_init()
{
	SWF_gNumCharacters = 0;
	SWF_versionNum = 5;

	Ming_cubicThreshold = 10000;
	Ming_scale = 20.0;

	return 0;
}


void
Ming_cleanup()
{
	Ming_cleanupFonts();
}

/*
 * Set output compression level.
 * This function sets the value of the compression level to be used when
 * generating output. The level should be a value between 1 and 9 inclusive
 * and corresponds to compression levels used by libz.
 * Returns previous value.
 */ 
int
Ming_setSWFCompression(int level /* new compression level */)
{
	int oldlevel = SWF_compression;
	SWF_compression = level;
	return oldlevel;
}

/*
 * Set the global scaling factor
 */
void Ming_setScale(float scale /* New scaling factor */)
{
	Ming_scale = scale;
}

/*
 * Get the global scaling factor
 * returns the current global scaling factor
 */
float Ming_getScale()
{
	return Ming_scale;
}

/*
 * Set the threshold used when approximating cubic quadratic shapes.
 */
void Ming_setCubicThreshold(int num /* New threshold value */)
{
	Ming_cubicThreshold = num;
}

/*
 * Set the function that gets called when a warning occurs within the library
 * This function sets function to be called when a warning occurs within the
 * library. The default function prints the warning message to stdout.
 * Returns the previously-set warning function.
 */
SWFMsgFunc Ming_setWarnFunction(SWFMsgFunc warn)
{
	return setSWFWarnFunction(warn);
}

/*
 * Set the function that gets called when an error occurs within the library
 * This function sets function to be called when an error occurs within the
 * library. The default function prints the error mesage to stdout and exits.
 * Returns the previously-set error function.
 */
SWFMsgFunc Ming_setErrorFunction(SWFMsgFunc error)
{
	return setSWFErrorFunction(error);
}

/*
 * set the version of SWF to produce on output
 * This function set the version of SWF to be produced by the library. Only
 * versions 4 through 8 inclusive are supported at this time.
 */
void Ming_useSWFVersion(int version /* Flash version */)
{
	static int called=0;

	if(version < 4 || version > 9)
		SWF_error("Only SWF versions 4 to 9 are currently supported!\n");

	if ( called && version != SWF_versionNum )
	{
		SWF_warn("WARNING: changing SWF target version during a run\n"
			 "         might result in malformed SWF output.\n"
			 "         You don't have to worry if you're careful about\n"
			 "         not mixing different version blocks in a movie.\n");
	}

	called=1;

	SWF_versionNum = version;

}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
