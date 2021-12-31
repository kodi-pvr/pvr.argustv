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

#include "platform.h"

namespace ArgusTV
{
class ATTR_DLL_LOCAL FileReader
{
public:
  FileReader() = default;
  virtual ~FileReader() = default;

  // Open and write to the file
  virtual std::string GetFileName() const;
  virtual long SetFileName(const std::string& fileName);
  virtual long OpenFile();
  virtual long CloseFile();
  virtual long Read(unsigned char* pbData, unsigned long lDataLength, unsigned long* dwReadBytes);
  virtual bool IsFileInvalid();
  virtual int64_t SetFilePointer(int64_t llDistanceToMove, unsigned long dwMoveMethod);
  virtual int64_t GetFilePointer();
  virtual void OnZap(void);
  virtual int64_t GetFileSize();
  virtual bool IsBuffer() { return false; };

  void SetDebugOutput(bool bDebugOutput);

protected:
  kodi::vfs::CFile m_file; // Handle to file for streaming
  std::string m_fileName; // The filename where we stream
  int64_t m_fileSize = 0;
  int64_t m_fileStartPos = 0;

  bool m_bDebugOutput = false;
};
} // namespace ArgusTV
