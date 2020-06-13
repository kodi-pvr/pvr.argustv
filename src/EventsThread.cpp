/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
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


CEventsThread::~CEventsThread(void)
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: destructor");
  // v17 Krypton. When exiting Kodi with this addon still subscribed,
  // network services are already unavailable. CArgusTV::UnsubscribeServiceEvents won't succeed
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

void* CEventsThread::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: thread started");
  while (!IsStopped() && m_subscribed)
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
    // The new P8PLATFORM:: thread library has a problem with stopping a thread that is doing a long sleep
    for (int i = 0; i < 100; i++)
    {
      if (Sleep(100))
        break;
    }
  }
  kodi::Log(ADDON_LOG_DEBUG, "CEventsThread:: thread stopped");
  return nullptr;
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
