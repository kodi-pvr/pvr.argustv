/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "upcomingrecording.h"

#include "argustvrpc.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

bool cUpcomingRecording::Parse(const Json::Value& data)
{
  int offset;
  std::string t;
  Json::Value channelobject, programobject;

  programobject = data["Program"];
  date = 0;

  id = programobject["Id"].asInt();
  t = programobject["StartTime"].asString();
  starttime = CArgusTV::WCFDateToTimeT(t, offset);
  t = programobject["StopTime"].asString();
  stoptime = CArgusTV::WCFDateToTimeT(t, offset);
  prerecordseconds = programobject["PreRecordSeconds"].asInt();
  postrecordseconds = programobject["PostRecordSeconds"].asInt();
  title = programobject["Title"].asString();
  iscancelled = programobject["IsCancelled"].asBool();
  upcomingprogramid = programobject["UpcomingProgramId"].asString();
  guideprogramid = programobject["GuideProgramId"].asString();
  scheduleid = programobject["ScheduleId"].asString();

  // From the Program class pickup the C# Channel class
  channelobject = programobject["Channel"];
  channelid = channelobject["ChannelId"].asString();
  channeldisplayname = channelobject["DisplayName"].asString();
  ichannelid = channelobject["Id"].asInt();

  if (data["CardChannelAllocation"].empty())
    isallocated = false;

  if (data["ConflictingPrograms"].empty())
    isinconflict = false;

  return true;
}
