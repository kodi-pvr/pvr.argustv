/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <json/json.h>
#include <string>

class cActiveRecording
{
public:
  cActiveRecording(void) = default;
  virtual ~cActiveRecording(void) = default;

  bool Parse(const Json::Value& data);

  const std::string& UpcomingProgramId(void) const { return upcomingprogramid; }

private:
  std::string upcomingprogramid;
};

