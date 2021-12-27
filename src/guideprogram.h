/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Marcel Groothuis, Fho
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "argustvrpc.h"

#include <json/json.h>
#include <string>

class ATTR_DLL_LOCAL cGuideProgram
{
public:
  cGuideProgram(void) = default;
  virtual ~cGuideProgram(void) = default;

  bool Parse(const Json::Value& data);

  const std::string& Actors(void) const { return actors; }
  const std::string& Category(void) const { return category; }
  const std::string& Description(void) const { return description; }
  const std::string& Directors(void) const { return directors; }
  int EpisodeNumber(void) const { return episodenumber; }
  const std::string& EpisodeNumberDisplay(void) const { return episodenumberdisplay; }
  int EpisodeNumberTotal(void) const { return episodenumbertotal; }
  int EpisodePart(void) const { return episodepart; }
  int EpisodePartTotal(void) const { return episodeparttotal; }
  const std::string& GuideChannelId(void) const { return guidechannelid; }
  const std::string& GuideProgramId(void) const { return guideprogramid; }
  bool IsChanged(void) const { return ischanged; }
  bool IsDeleted(void) const { return isdeleted; }
  bool IsPremiere(void) const { return ispremiere; }
  bool IsRepeat(void) const { return isrepeat; }
  const std::string& Rating(void) const { return rating; }
  int SeriesNumber(void) const { return seriesnumber; }
  double StarRating(void) const { return starrating; }
  time_t StartTime(void) const { return starttime; }
  time_t StopTime(void) const { return stoptime; }
  const std::string& SubTitle(void) const { return subtitle; }
  const std::string& Title(void) const { return title; }
  CArgusTV::VideoAspectRatio VideoAspect(void) const { return videoaspect; }

private:
  std::string actors;
  std::string category;
  std::string description;
  std::string directors;
  int episodenumber = 0;
  std::string episodenumberdisplay;
  int episodenumbertotal = 0;
  int episodepart = 0;
  int episodeparttotal = 0;
  std::string guidechannelid;
  std::string guideprogramid;
  bool ischanged = false;
  bool isdeleted = false;
  bool ispremiere = false;
  bool isrepeat = false;
  time_t lastmodifiedtime = 0;
  std::string rating;
  int seriesnumber = 0;
  double starrating = 0.0;
  time_t starttime = 0;
  time_t stoptime = 0;
  std::string subtitle;
  std::string title;
  CArgusTV::VideoAspectRatio videoaspect = CArgusTV::Unknown;
};
