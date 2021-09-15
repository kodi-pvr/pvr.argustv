/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

/*
 * Most of this code is taken from tools.c in the Video Disk Recorder ('VDR')
 */

#include "tools.h"

#include <kodi/General.h>

// --- cTimeMs ---------------------------------------------------------------

cTimeMs::cTimeMs(int Ms)
{
  Set(Ms);
}

void cTimeMs::Set(int Ms)
{
  m_begin = std::chrono::steady_clock::now() + std::chrono::milliseconds(Ms);
}

bool cTimeMs::TimedOut(void)
{
  return std::chrono::steady_clock::now() >= m_begin;
}

uint64_t cTimeMs::Elapsed(void)
{
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_begin).count();
}
