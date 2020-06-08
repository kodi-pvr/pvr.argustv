/*
 *      Copyright (C) 2010 Marcel Groothuis
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "KeepAliveThread.h"

#include "argustvrpc.h"
#include "pvrclient-argustv.h"
#include "utils.h"

#include "p8-platform/os.h"
#include <kodi/General.h>

CKeepAliveThread::CKeepAliveThread(cPVRClientArgusTV& instance) : m_instance(instance)
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: constructor");
}

CKeepAliveThread::~CKeepAliveThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: destructor");
}

void *CKeepAliveThread::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread started");
  while (!IsStopped())
  {
    int retval = m_instance.GetRPC().KeepLiveStreamAlive();
    kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: KeepLiveStreamAlive returned %i", (int) retval);
    // The new P8PLATFORM:: thread library has a problem with stopping a thread that is doing a long sleep
    for (int i = 0; i < 100; i++)
    {
      if (Sleep(100)) break;
    }
  }
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread stopped");
  return NULL;
}
