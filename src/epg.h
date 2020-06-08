#pragma once
/*
 *      Copyright (C) 2010 Marcel Groothuis
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

#include <kodi/AddonBase.h>
#include <json/json.h>
#include <string>

class cEpg
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
