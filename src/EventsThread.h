/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <atomic>
#include <json/json.h>
#include <kodi/AddonBase.h>
#include <thread>

class cPVRClientArgusTV;

class ATTR_DLL_LOCAL CEventsThread
{
public:
  CEventsThread(cPVRClientArgusTV& instance);
  ~CEventsThread();
  void Connect(void);

  void StartThread();
  void StopThread();

private:
  void Process();

  void HandleEvents(Json::Value events);

  bool m_subscribed = false;
  std::string m_monitorId;
  cPVRClientArgusTV& m_instance;
  std::atomic<bool> m_running = {false};
  std::thread m_thread;
};
