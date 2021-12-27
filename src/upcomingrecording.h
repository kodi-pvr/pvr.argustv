/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <json/json.h>
#include <kodi/AddonBase.h>
#include <string>

class ATTR_DLL_LOCAL cUpcomingRecording
{
public:
  cUpcomingRecording(void) = default;
  virtual ~cUpcomingRecording(void) = default;

  bool Parse(const Json::Value& data);

  int ID(void) const { return id; }
  const std::string& ChannelId(void) const { return channelid; }
  int ChannelID(void) const { return ichannelid; }
  const std::string& ChannelDisplayname(void) const { return channeldisplayname; }
  time_t StartTime(void) const { return starttime; }
  time_t StopTime(void) const { return stoptime; }
  int PreRecordSeconds(void) const { return prerecordseconds; }
  int PostRecordSeconds(void) const { return postrecordseconds; }
  const std::string& Title(void) const { return title; }
  bool IsCancelled(void) const { return iscancelled; }
  const std::string& UpcomingProgramId(void) const { return upcomingprogramid; }
  const std::string& GuideProgramId(void) const { return guideprogramid; }
  const std::string& ScheduleId(void) const { return scheduleid; }
  bool IsAllocated(void) const { return isallocated; }
  bool IsInConflict(void) const { return isinconflict; }

private:
  std::string channeldisplayname;
  std::string channelid;
  time_t date = 0;
  time_t starttime = 0;
  time_t stoptime = 0;
  int prerecordseconds = 0;
  int postrecordseconds = 0;
  std::string title;
  bool iscancelled = false;
  std::string upcomingprogramid;
  std::string guideprogramid;
  std::string scheduleid;
  bool isallocated = true;
  bool isinconflict = true;
  int id = 0;
  int ichannelid = 0;
};
