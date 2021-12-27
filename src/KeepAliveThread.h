/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <atomic>
#include <kodi/AddonBase.h>
#include <thread>

class cPVRClientArgusTV;

class ATTR_DLL_LOCAL CKeepAliveThread
{
public:
  CKeepAliveThread(cPVRClientArgusTV& instance);
  ~CKeepAliveThread();

  void StartThread();
  void StopThread();

private:
  void Process();

  cPVRClientArgusTV& m_instance;
  std::atomic<bool> m_running = {false};
  std::thread m_thread;
};
