/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "argustvrpc.h"

#include <json/json.h>
#include <string>

class ATTR_DLL_LOCAL cRecordingGroup
{
public:
  cRecordingGroup(void) = default;
  virtual ~cRecordingGroup(void) = default;

  bool Parse(const Json::Value& data);

  const std::string& Category(void) const { return category; }
  const std::string& ChannelDisplayName(void) const { return channeldisplayname; }
  const std::string& ChannelID(void) const { return channelid; }
  CArgusTV::ChannelType ChannelType(void) const { return channeltype; }
  bool IsRecording(void) const { return isrecording; }
  time_t LatestProgramStartTime(void) const { return latestprogramstarttime; }
  const std::string& ProgramTitle(void) const { return programtitle; }
  CArgusTV::RecordingGroupMode RecordingGroupMode(void) const { return recordinggroupmode; }
  int RecordingsCount(void) const { return recordingscount; }
  const std::string& ScheduleId(void) const { return scheduleid; }
  const std::string& ScheduleName(void) const { return schedulename; }
  CArgusTV::SchedulePriority SchedulePriority(void) const { return schedulepriority; }

private:
  std::string category;
  std::string channeldisplayname;
  std::string channelid;
  CArgusTV::ChannelType channeltype = CArgusTV::Television;
  bool isrecording = false;
  time_t latestprogramstarttime = 0;
  std::string programtitle;
  CArgusTV::RecordingGroupMode recordinggroupmode = CArgusTV::GroupByProgramTitle;
  int recordingscount = 0;
  std::string scheduleid;
  std::string schedulename;
  CArgusTV::SchedulePriority schedulepriority = CArgusTV::Normal;
};
