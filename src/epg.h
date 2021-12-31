/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <json/json.h>
#include <kodi/AddonBase.h>
#include <string>

class ATTR_DLL_LOCAL cEpg
{
public:
  cEpg() = default;
  virtual ~cEpg() = default;

  void Reset();

  bool Parse(const Json::Value& data);
  const std::string& UniqueId(void) const { return m_guideprogramid; }
  time_t StartTime(void) const { return m_starttime; }
  time_t EndTime(void) const { return m_endtime; }
  const std::string& Title(void) const { return m_title; }
  const std::string& Subtitle(void) const { return m_subtitle; }
  const std::string& Description(void) const { return m_description; }
  const std::string& Genre(void) const { return m_genre; }

private:
  std::string m_guideprogramid;
  std::string m_title;
  std::string m_subtitle;
  std::string m_description;
  std::string m_genre;
  time_t m_starttime = 0;
  time_t m_endtime = 0;
  time_t m_utcdiff = 0;
};
