//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : windows_compat.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @file windows_compat.h
 * Hacks to work with different versions of windows.
 * This is only included if WIN32 is set.
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#ifndef WINDOWS_COMPAT_H_INCLUDED
#define WINDOWS_COMPAT_H_INCLUDED

#define HAVE_SYS_STAT_H

#define NO_MACRO_VARARG

typedef char               CHAR;

typedef signed char        INT8;
typedef short              INT16;
typedef int                INT32;

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;

#ifndef PRIx64
#define PRIx64             "llx"
#endif

/* eliminate GNU's attribute */
#define __attribute__(x)

/* MSVC compilers before VC7 don't have __func__ at all; later ones call it
 * __FUNCTION__.
 */
#ifdef _MSC_VER
#if _MSC_VER < 1300
#define __func__    "???"
#else
#define __func__    __FUNCTION__
#endif
#else /* _MSC_VER */
#define __func__    "???"
#endif

#include "stdio.h"
#include "string.h"

#undef snprintf
#define snprintf      _snprintf

#undef vsnprintf
#define vsnprintf     _vsnprintf

#undef strcasecmp
#define strcasecmp    _strcmpi

#undef strncasecmp
#define strncasecmp   _strnicmp

#undef strdup
#define strdup        _strdup

#undef fileno
#define fileno        _fileno

/* includes for _setmode() */
#include <io.h>
#include <fcntl.h>
#include <direct.h>

#define mkdir(x, y) _mkdir(x)
#define PATH_SEP  '\\'

#endif   /* WINDOWS_COMPAT_H_INCLUDED */

