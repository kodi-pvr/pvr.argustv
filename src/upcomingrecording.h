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

#include <string>
#include <json/json.h>

class cUpcomingRecording
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
