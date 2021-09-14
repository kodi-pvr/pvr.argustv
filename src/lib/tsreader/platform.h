/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

/*
 *************************************************************************
 *  This file is a modified version from Team MediaPortal's
 *  TsReader DirectShow filter
 *  MediaPortal is a GPL'ed HTPC-Application
 *  Copyright (C) 2005-2012 Team MediaPortal
 *  http://www.team-mediaportal.com
 *
 * Changes compared to Team MediaPortal's version:
 * - Code cleanup for PVR addon usage
 * - Code refactoring for cross platform usage
 *************************************************************************
 *  This file originates from TSFileSource, a GPL directshow push
 *  source filter that provides an MPEG transport stream output.
 *  Copyright (C) 2005-2006 nate, bear
 *  http://forums.dvbowners.com/
 */

#pragma once

#include <kodi/Filesystem.h>

#if (defined(_WIN32) || defined(_WIN64))

#include <windows.h>
#include <wchar.h>

/* Handling of 2-byte Windows wchar strings */
#define WcsLen wcslen
#define WcsToMbs wcstombs
typedef wchar_t Wchar_t; /* sizeof(wchar_t) = 2 bytes on Windows */

#ifndef _SSIZE_T_DEFINED
#ifdef  _WIN64
typedef __int64    ssize_t;
#else
typedef _W64 int   ssize_t;
#endif
#define _SSIZE_T_DEFINED
#endif

/* Prevent deprecation warnings */
#ifndef snprintf
#define snprintf _snprintf
#endif
#define strnicmp _strnicmp

#else

#include <string.h>
#define strnicmp(X,Y,N) strncasecmp(X,Y,N)

#if defined(__APPLE__)
// for HRESULT
#include <CoreFoundation/CFPlugInCOM.h>
#endif

typedef long LONG;
#if !defined(__APPLE__)
typedef LONG HRESULT;
#endif

#define _FILE_OFFSET_BITS 64
#define FILE_BEGIN              0
#define FILE_CURRENT            1
#define FILE_END                2

#ifndef S_OK
#define S_OK           0L
#endif

#ifndef S_FALSE
#define S_FALSE        1L
#endif

#ifndef FAILED
#define FAILED(Status) ((HRESULT)(Status)<0)
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

// Error codes
#define ERROR_FILENAME_EXCED_RANGE 206L
#define ERROR_INVALID_NAME         123L

#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY              0x8007000EL
#endif

#ifndef E_FAIL
#define E_FAIL                     0x8004005EL
#endif

#ifdef TARGET_LINUX
#include <limits.h>
#define MAX_PATH PATH_MAX
#elif defined TARGET_DARWIN || defined __FreeBSD__
#include <sys/syslimits.h>
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH 256
#endif

/* Handling of 2-byte Windows wchar strings on non-Windows targets
 * Used by The MediaPortal and ForTheRecord pvr addons
 */
typedef uint16_t Wchar_t; /* sizeof(wchar_t) = 4 bytes on Linux, but the MediaPortal buffer files have 2-byte wchars */

/* This is a replacement of the Windows wcslen() function which assumes that
 * wchar_t is a 2-byte character.
 * It is used for processing Windows wchar strings
 */
inline size_t WcsLen(const Wchar_t *str)
{
  const unsigned short *eos = (const unsigned short*)str;
  while( *eos++ ) ;
  return( (size_t)(eos - (const unsigned short*)str) -1);
};

/* This is a replacement of the Windows wcstombs() function which assumes that
 * wchar_t is a 2-byte character.
 * It is used for processing Windows wchar strings
 */
inline size_t WcsToMbs(char *s, const Wchar_t *w, size_t n)
{
  size_t i = 0;
  const unsigned short *wc = (const unsigned short*) w;
  while(wc[i] && (i < n))
  {
    s[i] = wc[i];
    ++i;
  }
  if (i < n) s[i] = '\0';

  return (i);
};

#endif
