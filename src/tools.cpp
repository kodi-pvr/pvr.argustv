/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

/*
 * Most of this code is taken from tools.c in the Video Disk Recorder ('VDR')
 */

#include "tools.h"

#include <kodi/General.h>
#include <p8-platform/util/timeutils.h>

using namespace P8PLATFORM;

// --- cTimeMs ---------------------------------------------------------------

cTimeMs::cTimeMs(int Ms)
{
  Set(Ms);
}

uint64_t cTimeMs::Now(void)
{
#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
#define MIN_RESOLUTION 5 // ms
  static bool initialized = false;
  static bool monotonic = false;
  struct timespec tp;
  if (!initialized)
  {
    // check if monotonic timer is available and provides enough accurate resolution:
    if (clock_getres(CLOCK_MONOTONIC, &tp) == 0)
    {
      long Resolution = tp.tv_nsec;
      // require a minimum resolution:
      if (tp.tv_sec == 0 && tp.tv_nsec <= MIN_RESOLUTION * 1000000)
      {
        if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
        {
          kodi::Log(ADDON_LOG_DEBUG, "cTimeMs: using monotonic clock (resolution is %ld ns)",
                    Resolution);
          monotonic = true;
        }
        else
          kodi::Log(ADDON_LOG_ERROR, "cTimeMs: clock_gettime(CLOCK_MONOTONIC) failed");
      }
      else
        kodi::Log(ADDON_LOG_DEBUG,
                  "cTimeMs: not using monotonic clock - resolution is too bad (%ld s %ld ns)",
                  tp.tv_sec, tp.tv_nsec);
    }
    else
      kodi::Log(ADDON_LOG_ERROR, "cTimeMs: clock_getres(CLOCK_MONOTONIC) failed");
    initialized = true;
  }
  if (monotonic)
  {
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
      return (uint64_t(tp.tv_sec)) * 1000 + tp.tv_nsec / 1000000;
    kodi::Log(ADDON_LOG_ERROR, "cTimeMs: clock_gettime(CLOCK_MONOTONIC) failed");
    monotonic = false;
    // fall back to gettimeofday()
  }
#else
#if !defined(__WINDOWS__)
#warning Posix monotonic clock not available
#endif
#endif
  struct timeval t;
  if (gettimeofday(&t, NULL) == 0)
    return (uint64_t(t.tv_sec)) * 1000 + t.tv_usec / 1000;
  return 0;
}

void cTimeMs::Set(int Ms)
{
  begin = Now() + Ms;
}

bool cTimeMs::TimedOut(void)
{
  return Now() >= begin;
}

uint64_t cTimeMs::Elapsed(void)
{
  return Now() - begin;
}
