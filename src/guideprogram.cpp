/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Marcel Groothuis, Fho
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "guideprogram.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

bool cGuideProgram::Parse(const Json::Value& data)
{
  int offset;
  std::string t;
  //actors = data["Actors"].   .asString();
  category = data["Category"].asString();
  description = data["Description"].asString();
  //directors = data["Directors"].asString();
  episodenumber = data["EpisodeNumber"].asInt();
  episodenumberdisplay = data["EpisodeNumberDisplay"].asString();
  episodenumbertotal = data["EpisodeNumberTotal"].asInt();
  episodepart = data["EpisodePart"].asInt();
  episodeparttotal = data["EpisodePartTotal"].asInt();
  guidechannelid = data["GuideChannelId"].asString();
  guideprogramid = data["GuideProgramId"].asString();
  ischanged = data["IsChanged"].asBool();
  isdeleted = data["IsDeleted"].asBool();
  ispremiere = data["IsPremiere"].asBool();
  isrepeat = data["IsRepeat"].asBool();
  t = data["LastModifiedTime"].asString();
  lastmodifiedtime = CArgusTV::WCFDateToTimeT(t, offset);
  lastmodifiedtime += ((offset / 100) * 3600);
  rating = data["Rating"].asString();
  seriesnumber = data["SeriesNumber"].asInt();
  starrating = data["StarRating"].asDouble();
  t = data["StartTime"].asString();
  starttime = CArgusTV::WCFDateToTimeT(t, offset);
  starttime += ((offset / 100) * 3600);
  t = data["StopTime"].asString();
  stoptime = CArgusTV::WCFDateToTimeT(t, offset);
  stoptime += ((offset / 100) * 3600);
  subtitle = data["SubTitle"].asString();
  title = data["Title"].asString();
  videoaspect = static_cast<CArgusTV::VideoAspectRatio>(data["VideoAspect"].asInt());

  return true;
}
