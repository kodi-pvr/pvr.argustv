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

#include "FileReader.h"

#include <string>
#include <vector>

namespace ArgusTV
{
class ATTR_DLL_LOCAL MultiFileReaderFile
{
public:
  std::string filename;
  int64_t startPosition;
  int64_t length;
  long filePositionId;
};

class ATTR_DLL_LOCAL MultiFileReader : public FileReader
{
public:
  MultiFileReader() = default;
  ~MultiFileReader() override = default;

  std::string GetFileName() const override;
  long SetFileName(const std::string& fileName) override;
  long OpenFile() override;
  long CloseFile() override;
  long Read(unsigned char* pbData, unsigned long lDataLength, unsigned long* dwReadBytes) override;
  bool IsFileInvalid() override;

  int64_t SetFilePointer(int64_t llDistanceToMove, unsigned long dwMoveMethod) override;
  int64_t GetFilePointer() override;
  int64_t GetFileSize() override;
  void OnZap(void) override;

protected:
  long RefreshTSBufferFile();
  long GetFileLength(const std::string& filename, int64_t& length);

  FileReader m_TSBufferFile;
  int64_t m_startPosition = 0;
  int64_t m_endPosition = 0;
  int64_t m_currentReadPosition = 0;
  long m_filesAdded = 0;
  long m_filesRemoved = 0;
  int64_t m_lastZapPosition = 0;

  std::vector<MultiFileReaderFile*> m_tsFiles;

  FileReader m_TSFile;
  long m_TSFileId = 0;
  bool m_bDelay = false;
  bool m_bDebugOutput = false;
};
} // namespace ArgusTV
