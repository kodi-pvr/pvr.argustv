/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <json/json.h>
#include <p8-platform/threads/threads.h>

class cPVRClientArgusTV;

class CEventsThread : public P8PLATFORM::CThread
{
public:
  CEventsThread(cPVRClientArgusTV& instance);
  ~CEventsThread(void);
  void Connect(void);

private:
  virtual void* Process(void);

  void HandleEvents(Json::Value events);

  bool m_subscribed = false;
  std::string m_monitorId;
  cPVRClientArgusTV& m_instance;
};
