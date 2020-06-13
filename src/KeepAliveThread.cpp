/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "KeepAliveThread.h"

#include "argustvrpc.h"
#include "pvrclient-argustv.h"
#include "utils.h"

#include <kodi/General.h>
#include <p8-platform/os.h>

CKeepAliveThread::CKeepAliveThread(cPVRClientArgusTV& instance) : m_instance(instance)
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: constructor");
}

CKeepAliveThread::~CKeepAliveThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: destructor");
}

void* CKeepAliveThread::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread started");
  while (!IsStopped())
  {
    int retval = m_instance.GetRPC().KeepLiveStreamAlive();
    kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: KeepLiveStreamAlive returned %i", (int)retval);
    // The new P8PLATFORM:: thread library has a problem with stopping a thread that is doing a long sleep
    for (int i = 0; i < 100; i++)
    {
      if (Sleep(100))
        break;
    }
  }
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread stopped");
  return nullptr;
}
