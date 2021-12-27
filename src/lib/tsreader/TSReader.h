/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

/*
 *************************************************************************
 *  Parts of this file originate from Team MediaPortal's
 *  TsReader DirectShow filter
 *  MediaPortal is a GPL'ed HTPC-Application
 *  Copyright (C) 2005-2012 Team MediaPortal
 *  http://www.team-mediaportal.com
 *
 * Changes compared to Team MediaPortal's version:
 * - Code cleanup for PVR addon usage
 * - Code refactoring for cross platform usage
 *************************************************************************/

#pragma once

#include "FileReader.h"

#if defined(TARGET_WINDOWS)
#include <windows.h>
#endif

namespace ArgusTV
{
class ATTR_DLL_LOCAL CTsReader
{
public:
  CTsReader();
  ~CTsReader(void) = default;
  long Open(const std::string& fileName);
  long Read(unsigned char* pbData, unsigned long lDataLength, unsigned long* dwReadBytes);
  void Close();
  int64_t SetFilePointer(int64_t llDistanceToMove, unsigned long dwMoveMethod);
  int64_t GetFileSize();
  int64_t GetFilePointer();
  void OnZap(void);
#if defined(TARGET_WINDOWS)
  long long sigmaTime();
  long long sigmaCount();
#endif

private:
  bool m_bTimeShifting = false;
  bool m_bRecording = false;
  bool m_bLiveTv = false;
  std::string m_fileName;
  FileReader* m_fileReader = nullptr;
#if defined(TARGET_WINDOWS)
  LARGE_INTEGER liDelta;
  LARGE_INTEGER liCount;
#endif
};
} // namespace ArgusTV
