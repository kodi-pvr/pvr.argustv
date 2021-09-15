/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
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

CKeepAliveThread::CKeepAliveThread(cPVRClientArgusTV& instance) : m_instance(instance)
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: constructor");
}

CKeepAliveThread::~CKeepAliveThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: destructor");
  StopThread();
}

void CKeepAliveThread::StartThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: start");

  if (!m_running)
  {
    m_running = true;
    m_thread = std::thread([&] { Process(); });
  }
}

void CKeepAliveThread::StopThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: stop");
  if (m_running)
  {
    m_running = false;
    if (m_thread.joinable())
      m_thread.join();
  }
}

void CKeepAliveThread::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread started");
  while (m_running)
  {
    int retval = m_instance.GetRPC().KeepLiveStreamAlive();
    kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: KeepLiveStreamAlive returned %i", (int)retval);

    for (int i = 0; i < 100; i++)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (!m_running)
        break;
    }
  }
  kodi::Log(ADDON_LOG_DEBUG, "CKeepAliveThread:: thread stopped");
}
