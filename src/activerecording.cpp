/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "activerecording.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

// This is a minimalistic parser, parsing only the fields that
// are currently used by the implementation
bool cActiveRecording::Parse(const Json::Value& data)
{
  // From the Active Recording class pickup the Program class
  Json::Value programobject;
  programobject = data["Program"];

  // Then, from the Program class, pick up the upcoming program id
  upcomingprogramid = programobject["UpcomingProgramId"].asString();

  return true;
}
