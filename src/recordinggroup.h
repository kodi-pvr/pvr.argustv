#pragma once
/*
 *      Copyright (C) 2011 Fred Hoogduin
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "argustvrpc.h"

#include <json/json.h>
#include <string>

class cRecordingGroup
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
