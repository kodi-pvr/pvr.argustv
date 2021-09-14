/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Marcel Groothuis, Fho
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "recording.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

bool cRecording::Parse(const Json::Value& data)
{
  int offset;
  std::string t;
  id = data["Id"].asInt();
  actors = data["Actors"].asString();
  category = data["Category"].asString();
  channeldisplayname = data["ChannelDisplayName"].asString();
  channelid = data["ChannelId"].asString();
  channeltype = (CArgusTV::ChannelType)data["ChannelType"].asInt();
  description = data["Description"].asString();
  director = data["Director"].asString();
  episodenumber = data["EpisodeNumber"].asInt();
  episodenumberdisplay = data["EpisodeNumberDisplay"].asString();
  episodenumbertotal = data["EpisodeNumberTotal"].asInt();
  episodepart = data["EpisodePart"].asInt();
  episodeparttotal = data["EpisodePartTotal"].asInt();
  isfullywatched = data["IsFullyWatched"].asBool();
  ispartofseries = data["IsPartOfSeries"].asBool();
  ispartialrecording = data["IsPartialRecording"].asBool();
  ispremiere = data["IsPremiere"].asBool();
  isrepeat = data["IsRepeat"].asBool();
  keepuntilmode = (CArgusTV::KeepUntilMode)data["KeepUntilMode"].asInt();
  keepuntilvalue = data["KeepUntilValue"].asInt();
  lastwatchedposition = data["LastWatchedPosition"].asInt();
  fullywatchedcount = data["FullyWatchedCount"].asInt();
  t = data["LastWatchedTime"].asString();
  lastwatchedtime = CArgusTV::WCFDateToTimeT(t, offset);
  t = data["ProgramStartTime"].asString();
  programstarttime = CArgusTV::WCFDateToTimeT(t, offset);
  t = data["ProgramStopTime"].asString();
  programstoptime = CArgusTV::WCFDateToTimeT(t, offset);
  rating = data["Rating"].asString();
  recordingfileformatid = data["RecordingFileFormatId"].asString();
  t = data["RecordingFileName"].asString();
  recordingfilename = ToCIFS(t);
  recordingid = data["RecordingId"].asString();
  t = data["RecordingStartTime"].asString();
  recordingstarttime = CArgusTV::WCFDateToTimeT(t, offset);
  t = data["RecordingStopTime"].asString();
  recordingstoptime = CArgusTV::WCFDateToTimeT(t, offset);
  scheduleid = data["ScheduleId"].asString();
  schedulename = data["ScheduleName"].asString();
  schedulepriority = (CArgusTV::SchedulePriority)data["SchedulePriority"].asInt();
  seriesnumber = data["SeriesNumber"].asInt();
  starrating = data["StarRating"].asDouble();
  subtitle = data["SubTitle"].asString();
  title = data["Title"].asString();

  return true;
}

// Ok, this recording is part of a group of recordings, we do some
// title etc. juggling to make the listing more attractive
void cRecording::Transform(bool isgroupmember)
{
  std::string _title = title;
  std::string _subtitle = subtitle;

  if (isgroupmember)
  {
    if (subtitle.size() > 0)
    {
      title = _title + " - " + _subtitle;
      subtitle = channeldisplayname;
    }
    else
    {
      title = _title + " - " + channeldisplayname;
    }
  }
  else
  {
    if (subtitle.size() == 0)
    {
      subtitle = channeldisplayname;
    }
  }
}
