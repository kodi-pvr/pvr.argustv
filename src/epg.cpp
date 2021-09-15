/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "epg.h"

#include "addon.h"
#include "pvrclient-argustv.h"
#include "utils.h"

#include <kodi/General.h>
#include <stdio.h>
#include <vector>

void cEpg::Reset()
{
  m_guideprogramid.clear();
  m_title.clear();
  m_subtitle.clear();
  m_description.clear();
  m_genre.clear();

  m_starttime = 0;
  m_endtime = 0;
}

bool cEpg::Parse(const Json::Value& data)
{
  try
  {
    int offset;
    // All possible fields:
    //.Category=""
    //.EpisodeNumber=null
    //.EpisodeNumberDisplay=""
    //.EpisodeNumberTotal=null
    //.EpisodePart=null
    //.EpisodePartTotal=null
    //.GuideChannelId="26aa19b2-9d5d-4549-9ad8-ab6b908d6127"
    //.GuideProgramId="5bd17a57-f1f7-df11-862d-005056c00008"
    //.IsPremiere=false
    //.IsRepeat=false
    //.Rating=""
    //.SeriesNumber=null
    //.StarRating=null
    //.StartTime="/Date(1290896700000+0100)/" Database: 2010-11-27 23:25:00
    //.StopTime="/Date(1290899100000+0100)/"  Database: 2010-11-28 00:05:00
    //.SubTitle=""
    //.Title="NOS Studio Sport"
    //.VideoAspect=0
    m_guideprogramid = data["GuideProgramId"].asString();
    m_title = data["Title"].asString();
    m_subtitle = data["SubTitle"].asString();
    // TODO: Until the xbmc EPG gui starts using the episode names, we add them to the title
    if (m_subtitle.size() > 0)
    {
      m_title = m_title + " (" + m_subtitle + ")";
    }
    m_description = data["Description"].asString();
    m_genre = data["Category"].asString();

    // Dates are returned in a WCF compatible format ("/Date(9991231231+0100)/")
    std::string starttime = data["StartTime"].asString();
    std::string endtime = data["StopTime"].asString();

    m_starttime = CArgusTV::WCFDateToTimeT(starttime, offset);
    m_endtime = CArgusTV::WCFDateToTimeT(endtime, offset);

    //kodi::Log(ADDON_LOG_DEBUG, "Program: %s,%s Start: %s", m_title.c_str(), m_subtitle.c_str(), ctime(&m_starttime));
    //kodi::Log(ADDON_LOG_DEBUG, "End: %s", ctime(&m_endtime));

    return true;
  }
  catch (std::exception& e)
  {
    kodi::Log(ADDON_LOG_ERROR, "Exception '%s' during parse EPG json data.", e.what());
  }

  return false;
}
