/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "channel.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <vector>

bool cChannel::Parse(const Json::Value& data)
{
  //Json::printValueTree(data);

  name = data["DisplayName"].asString();
  type = (CArgusTV::ChannelType)data["ChannelType"].asInt();
  lcn = data["LogicalChannelNumber"].asInt();
  id = data["Id"].asInt();
  guid = data["ChannelId"].asString();
  guidechannelid = data["GuideChannelId"].asString();

  return true;
}
