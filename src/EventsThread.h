/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <atomic>
#include <kodi/AddonBase.h>
#include <thread>

#include <json/json.h>

class cPVRClientArgusTV;

class ATTRIBUTE_HIDDEN CEventsThread
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
