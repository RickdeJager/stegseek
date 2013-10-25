/*
 * steghide 0.5.1 - a steganography program
 * Copyright (C) 1999-2003 Stefan Hetzl <shetzl@chello.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <cstdlib>
#include <clocale>

#include "common.h"
#include "Session.h"
#include "SteghideError.h"

#ifdef WIN32 // locale support on Windows

#include <windows.h>

typedef struct struct_LCIDENTRY {
	LCID localeID ;
	char language[3] ;
} LCIDENTRY ;

LCIDENTRY LCIDTable[] = {
	// french
	{ 0x040c, "fr" },	// France
	{ 0x080c, "fr" },	// Belgium
	{ 0x0c0c, "fr" },	// Canada
	{ 0x100c, "fr" },	// Switzerland
	{ 0x140c, "fr" },	// Luxembourg
	{ 0x180c, "fr" },	// Monaco
	// german
	{ 0x0407, "de" },	// Germany
	{ 0x0807, "de" },	// Switzerland
	{ 0x0c07, "de" },	// Austria
	{ 0x1007, "de" },	// Luxembourg
	{ 0x1407, "de" },	// Liechtenstein
	// spanish
	{ 0x040a, "es" },	// Spain - Traditional
	{ 0x080a, "es" },	// Mexico
	{ 0x0c0a, "es" },	// Spain - Modern Sort
	{ 0x100a, "es" },	// Guatemala
	{ 0x140a, "es" },	// Costa Rica
	{ 0x180a, "es" },	// Panama
	{ 0x1c0a, "es" },	// Dominican Republic
	{ 0x200a, "es" },	// Venezuela
	{ 0x240a, "es" },	// Colombia
	{ 0x280a, "es" },	// Peru
	{ 0x2c0a, "es" },	// Argentinia
	{ 0x300a, "es" },	// Ecuador
	{ 0x340a, "es" },	// Chile
	{ 0x380a, "es" },	// Uruguay
	{ 0x3c0a, "es" },	// Paraguay
	{ 0x400a, "es" },	// Bolivia
	{ 0x440a, "es" },	// El Salvador
	{ 0x480a, "es" },	// Honduras
	{ 0x4c0a, "es" },	// Nicaragua
	{ 0x500a, "es" },	// Puerto Rico
	// romanian
	{ 0x0418, "ro" },	// Romanian
	{ 0x0818, "ro" },	// Romanian (Moldova)
	// end of LCIDTable
	{ 0x0000, "__" }
} ;

#undef LOCALEDIR
#define LOCALEDIR	"./locale/"

#endif // WIN32

static void gettext_init (void) ;

int main (int argc, char *argv[])
{
	try {
		gettext_init() ;

		Args = Arguments (argc, argv) ;
		Args.parse() ;

		Session s ;
		s.run() ;
	}
	catch (SteghideError& e) {
		e.printMessage() ;
		exit(EXIT_FAILURE) ;
	}

	exit(EXIT_SUCCESS) ;
}

static void gettext_init (void)
{
#ifndef DEBUG
	/* initialize gettext */
	setlocale (LC_ALL, "") ;
	bindtextdomain (PACKAGE, LOCALEDIR) ;
	bind_textdomain_codeset (PACKAGE, "ISO-8859-1") ;
	textdomain (PACKAGE) ;

#ifdef WIN32
	/* using the Windows API to find out which language should be used
	   (as there is no environment variable indicating the language) */
	{
		LCID localeID = GetThreadLocale () ;	
		int i = 0 ;

		while (LCIDTable[i].localeID != 0x0000) {
			if (localeID == LCIDTable[i].localeID) {
				setenv ("LANG", LCIDTable[i].language, 1) ;
				/* Make Change known (see gettext manual) */
				{
					extern int _nl_msg_cat_cntr ;
					++_nl_msg_cat_cntr;
				}
				break ;
			}

			i++ ;
		}
	}
#endif // ndef WIN32
#endif // ndef DEBUG
}
