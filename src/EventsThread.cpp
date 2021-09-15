/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "EventsThread.h"

#include "argustvrpc.h"
#include "pvrclient-argustv.h"

#include <kodi/General.h>

CEventsThread::CEventsThread(cPVRClientArgusTV& instance) : m_instance(instance)
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: constructor");
}


CEventsThread::~CEventsThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: destructor");
  // v17 Krypton. When exiting Kodi with this addon still subscribed,
  // network services are already unavailable. CArgusTV::UnsubscribeServiceEvents won't succeed
  StopThread();
}

void CEventsThread::StartThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: start");

  if (!m_running)
  {
    m_running = true;
    m_thread = std::thread([&] { Process(); });
  }
}

void CEventsThread::StopThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: stop");
  if (m_running)
  {
    m_running = false;
    if (m_thread.joinable())
      m_thread.join();
  }
}

void CEventsThread::Connect()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread::Connect");
  // Subscribe to service events
  Json::Value response;
  int retval = m_instance.GetRPC().SubscribeServiceEvents(CArgusTV::AllEvents, response);
  if (retval >= 0)
  {
    m_monitorId = response.asString();
    m_subscribed = true;
    kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: monitorId = %s", m_monitorId.c_str());
  }
  else
  {
    m_subscribed = false;
    kodi::Log(ADDON_LOG_INFO, "CEventsThread:: subscribe to events failed");
  }
}

void CEventsThread::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: thread started");
  while (m_running && m_subscribed)
  {
    // Get service events
    Json::Value response;
    int retval = m_instance.GetRPC().GetServiceEvents(m_monitorId, response);
    if (retval >= 0)
    {
      if (response["Expired"].asBool())
      {
        // refresh subscription
        Connect();
      }
      else
      {
        // Process service events
        Json::Value events = response["Events"];
        if (events.size() > 0u)
          HandleEvents(events);
      }
    }

    for (int i = 0; i < 100; i++)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (!m_running)
        break;
    }
  }
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: thread stopped");
}

void CEventsThread::HandleEvents(Json::Value events)
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread::HandleEvents");
  int size = events.size();
  bool mustUpdateTimers = false;
  bool mustUpdateRecordings = false;
  // Aggregate events
  for (int i = 0; i < size; i++)
  {
    Json::Value event = events[i];
    std::string eventName = event["Name"].asString();
    kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: ARGUS TV reports event %s", eventName.c_str());
    if (eventName == "UpcomingRecordingsChanged")
    {
      kodi::Log(ADDON_LOG_DEBUG, "Timers changed");
      mustUpdateTimers = true;
    }
    else if (eventName == "RecordingStarted" || eventName == "RecordingEnded")
    {
      kodi::Log(ADDON_LOG_DEBUG, "Recordings changed");
      mustUpdateRecordings = true;
    }
  }
  // Handle aggregated events
  if (mustUpdateTimers)
  {
    kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: Timers update triggered");
    m_instance.TriggerTimerUpdate();
  }
  if (mustUpdateRecordings)
  {
    kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: Recordings update triggered");
    m_instance.TriggerRecordingUpdate();
  }
}
