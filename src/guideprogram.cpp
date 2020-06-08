/*
*      Copyright (C) 2011 Marcel Groothuis, Fho
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

#include "guideprogram.h"

#include <stdlib.h>
#include <string.h>
#include "utils.h"
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
  lastmodifiedtime += ((offset/100)*3600);
  rating = data["Rating"].asString();
  seriesnumber = data["SeriesNumber"].asInt();
  starrating = data["StarRating"].asDouble();
  t = data["StartTime"].asString();
  starttime = CArgusTV::WCFDateToTimeT(t, offset);
  starttime += ((offset/100)*3600);
  t = data["StopTime"].asString();
  stoptime = CArgusTV::WCFDateToTimeT(t, offset);
  stoptime += ((offset/100)*3600);
  subtitle = data["SubTitle"].asString();
  title = data["Title"].asString();
  videoaspect = static_cast<CArgusTV::VideoAspectRatio>(data["VideoAspect"].asInt());

  return true;
}
