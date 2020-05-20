/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://www.xbmc.org
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

#include "FileReader.h"
#include <algorithm> //std::min, std::max
#include "p8-platform/util/timeutils.h" // for usleep

#include <kodi/General.h>

namespace ArgusTV
{

  std::string FileReader::GetFileName() const
  {
    return m_fileName;
  }

  long FileReader::SetFileName(const std::string& fileName)
  {
    m_fileName = fileName;
    return S_OK;
  }

  //
  // OpenFile
  //
  // Opens the file ready for streaming
  //
  long FileReader::OpenFile()
  {
    int Tmo = 25; //5 in MediaPortal

    // Is the file already opened
    if (!IsFileInvalid())
    {
      kodi::Log(ADDON_LOG_INFO, "FileReader::OpenFile() file already open");
      return S_OK;
    }

    // Has a filename been set yet
    if (m_fileName.empty())
    {
      kodi::Log(ADDON_LOG_ERROR, "FileReader::OpenFile() no filename");
      return ERROR_INVALID_NAME;
    }

    kodi::Log(ADDON_LOG_DEBUG, "FileReader::OpenFile() Trying to open %s", m_fileName.c_str());

    do
    {
      kodi::Log(ADDON_LOG_INFO, "FileReader::OpenFile() %s.", m_fileName.c_str());
      if (m_file.OpenFile(m_fileName, ADDON_READ_CHUNKED))
      {
        break;
      }

      // Is this still needed on Windows?
      //CStdStringW strWFile = UTF8Util::ConvertUTF8ToUTF16(m_fileName.c_str());
      usleep(20000);
    } while (--Tmo);

    if (Tmo)
    {
      if (Tmo < 4) // 1 failed + 1 succeded is quasi-normal, more is a bit suspicious ( disk drive too slow or problem ? )
        kodi::Log(ADDON_LOG_DEBUG, "FileReader::OpenFile(), %d tries to succeed opening %ws.", 6 - Tmo, m_fileName.c_str());
    }
    else
    {
      kodi::Log(ADDON_LOG_ERROR, "FileReader::OpenFile(), open file %s failed.", m_fileName.c_str());
      return S_FALSE;
    }

    kodi::Log(ADDON_LOG_DEBUG, "%s: OpenFile(%s) succeeded.", __FUNCTION__, m_fileName.c_str());

    return S_OK;

  } // Open

  //
  // CloseFile
  //
  // Closes any dump file we have opened
  //
  long FileReader::CloseFile()
  {
    if (IsFileInvalid())
    {
      return S_OK;
    }

    m_file.Close();

    return S_OK;
  } // CloseFile


  inline bool FileReader::IsFileInvalid()
  {
    return !m_file.IsOpen();
  }

  int64_t FileReader::SetFilePointer(int64_t llDistanceToMove, unsigned long dwMoveMethod)
  {
    //kodi::Log(ADDON_LOG_DEBUG, "%s: distance %d method %d.", __FUNCTION__, llDistanceToMove, dwMoveMethod);
    int64_t rc = m_file.Seek(llDistanceToMove, dwMoveMethod);
    //kodi::Log(ADDON_LOG_DEBUG, "%s: distance %d method %d returns %d.", __FUNCTION__, llDistanceToMove, dwMoveMethod, rc);
    return rc;
  }


  int64_t FileReader::GetFilePointer()
  {
    return m_file.GetPosition();
  }


  long FileReader::Read(unsigned char* pbData, unsigned long lDataLength, unsigned long *dwReadBytes)
  {
    *dwReadBytes = m_file.Read((void*)pbData, lDataLength);//Read file data into buffer
    //kodi::Log(ADDON_LOG_DEBUG, "%s: requested read length %d actually read %d.", __FUNCTION__, lDataLength, *dwReadBytes);

    if (*dwReadBytes < lDataLength)
    {
      kodi::Log(ADDON_LOG_DEBUG, "FileReader::Read() read too less bytes");
      return S_FALSE;
    }
    return S_OK;
  }

  void FileReader::SetDebugOutput(bool bDebugOutput)
  {
    m_bDebugOutput = bDebugOutput;
  }

  int64_t FileReader::GetFileSize()
  {
    return m_file.GetLength();
  }

  void FileReader::OnZap(void)
  {
    SetFilePointer(0, FILE_END);
  }
}
