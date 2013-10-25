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

#ifndef SH_SGI_HASH_MAP_H
#define SH_SGI_HASH_MAP_H

// this file is included to get access to a sgi::hash_map implementation

#ifdef __GNUC__
# if __GNUC__ < 3
#  include <hash_map.h>
    namespace sgi { using ::hash ; using ::hash_map ; } ;
# else
#  include <ext/hash_map>
#  if __GNUC_MINOR__ == 0
    namespace sgi = std ;			// GCC 3.0
#  else
    namespace sgi = __gnu_cxx ;	// GCC 3.1 and later
#  endif
# endif
#else
  namespace sgi = std ;
#endif

#endif // ndef SH_SGI_HASH_MAP_H
