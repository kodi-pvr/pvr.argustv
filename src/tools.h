/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#ifndef __TOOLS_H
#define __TOOLS_H

#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <kodi/AddonBase.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ERRNUL(e) \
  { \
    errno = e; \
    return 0; \
  }
#define ERRSYS(e) \
  { \
    errno = e; \
    return -1; \
  }

#define SECSINDAY 86400

#define KILOBYTE(n) ((n)*1024)
#define MEGABYTE(n) ((n)*1024LL * 1024LL)

#define MALLOC(type, size) (type*)malloc(sizeof(type) * (size))

#define DELETENULL(p) (delete (p), p = nullptr)
//
//#define CHECK(s) { if ((s) < 0) LOG_ERROR; } // used for 'ioctl()' calls
#define FATALERRNO (errno && errno != EAGAIN && errno != EINTR)

class ATTR_DLL_LOCAL cTimeMs
{
private:
  std::chrono::steady_clock::time_point m_begin;

public:
  cTimeMs(int Ms = 0);
  void Set(int Ms = 0);
  bool TimedOut(void);
  uint64_t Elapsed(void);
};


#endif //__TOOLS_H
