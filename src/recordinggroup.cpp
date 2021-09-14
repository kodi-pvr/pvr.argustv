/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "recordinggroup.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

bool cRecordingGroup::Parse(const Json::Value& data)
{
  //Json::printValueTree(data);

  category = data["Category"].asString();
  channeldisplayname = data["ChannelDisplayName"].asString();
  channelid = data["ChannelId"].asString();
  channeltype = (CArgusTV::ChannelType)data["ChannelType"].asInt();
  isrecording = data["IsRecording"].asBool();
  int offset;
  std::string lpst = data["LatestProgramStartTime"].asString();
  latestprogramstarttime = CArgusTV::WCFDateToTimeT(lpst, offset);
  latestprogramstarttime += ((offset / 100) * 3600);
  programtitle = data["ProgramTitle"].asString();
  recordinggroupmode = (CArgusTV::RecordingGroupMode)data["RecordingGroupMode"].asInt();
  recordingscount = data["RecordingsCount"].asInt();
  scheduleid = data["ScheduleId"].asString();
  schedulename = data["ScheduleName"].asString();
  schedulepriority = (CArgusTV::SchedulePriority)data["SchedulePriority"].asInt();

  return true;
}
