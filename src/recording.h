#pragma once
/*
 *      Copyright (C) 2011 Marcel Groothuis, FHo
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
#include "argustvrpc.h"

class cRecording
{
public:
  cRecording(void) = default;
  virtual ~cRecording(void) = default;

  bool Parse(const Json::Value& data);

  void Transform(bool isgroupmember);
  int Id(void) const { return id; }
  const std::string& Actors(void) const { return actors; }
  const std::string& Category(void) const { return category; }
  const std::string& ChannelDisplayName(void) const { return channeldisplayname; }
  const std::string& ChannelId(void) const { return channelid; }
  CArgusTV::ChannelType ChannelType(void) const { return channeltype; };
  const std::string& Description(void) const { return description; }
  const std::string& Director(void) const { return director; }
  int EpisodeNumber(void) const { return episodenumber; }
  const std::string& EpisodeNumberDisplay(void) const { return episodenumberdisplay; }
  int EpisodeNumberTotal(void) const { return episodenumbertotal; }
  int EpisodePart(void) const { return episodepart; }
  int EpisodePartTotal(void) const { return episodeparttotal; }
  bool IsFullyWatched(void) const { return isfullywatched; }
  bool IsPartOfSeries(void) const { return ispartofseries; }
  bool IsPartialRecording(void) const { return ispartialrecording; }
  bool IsPremiere(void) const { return ispremiere; }
  bool IsRepeat(void) const { return isrepeat; }
  CArgusTV::KeepUntilMode KeepUntilMode(void) const { return keepuntilmode; }
  int KeepUntilValue(void) const { return keepuntilvalue; }
  int LastWatchedPosition(void) const { return lastwatchedposition; }
  int FullyWatchedCount(void) const { return fullywatchedcount; }
  time_t LastWatchedTime(void) const { return lastwatchedtime; }
  time_t ProgramStartTime(void) const { return programstarttime; }
  time_t ProgramStopTime(void) const { return programstoptime; }
  const std::string& Rating(void) const { return rating; }
  const std::string& RecordingFileFormatId(void) const { return recordingfileformatid; }
  const std::string& RecordingFileName(void) const { return recordingfilename; }
  const std::string& RecordingId(void) const { return recordingid; }
  time_t RecordingStartTime(void) const { return recordingstarttime; }
  time_t RecordingStopTime(void) const { return recordingstoptime; }
  const std::string& ScheduleId(void) const { return scheduleid; }
  const std::string& ScheduleName(void) const { return schedulename; }
  CArgusTV::SchedulePriority SchedulePriority(void) const { return schedulepriority; }
  int SeriesNumber(void) const { return seriesnumber; }
  double StarRating(void) const { return starrating; }
  const std::string& SubTitle(void) const { return subtitle; }
  const std::string& Title(void) const { return title; }

private:
  int id = 0;
  std::string actors;
  std::string category;
  std::string channeldisplayname;
  std::string channelid;
  CArgusTV::ChannelType channeltype = CArgusTV::Television;
  std::string description;
  std::string director;
  int episodenumber = 0;
  std::string episodenumberdisplay;
  int episodenumbertotal = 0;
  int episodepart = 0;
  int episodeparttotal = 0;
  bool isfullywatched = false;
  bool ispartofseries = false;
  bool ispartialrecording = false;
  bool ispremiere = false;
  bool isrepeat = false;
  CArgusTV::KeepUntilMode keepuntilmode = CArgusTV::UntilSpaceIsNeeded;
  int keepuntilvalue = 0;
  int lastwatchedposition = 0;
  int fullywatchedcount = 0;
  time_t lastwatchedtime = 0;
  time_t programstarttime = 0;
  time_t programstoptime = 0;
  std::string rating;
  std::string recordingfileformatid;
  std::string recordingfilename;
  std::string recordingid;
  time_t recordingstarttime = 0;
  time_t recordingstoptime = 0;
  std::string scheduleid;
  std::string schedulename;
  CArgusTV::SchedulePriority schedulepriority = CArgusTV::Normal;
  int seriesnumber = 0;
  double starrating = 0.0;
  std::string subtitle;
  std::string title;
};
