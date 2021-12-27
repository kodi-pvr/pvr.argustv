/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "argustvrpc.h"

#include <json/json.h>
#include <string>

class ATTR_DLL_LOCAL cChannel
{
public:
  cChannel() = default;
  virtual ~cChannel() = default;

  bool Parse(const Json::Value& data);
  const std::string& Name(void) const { return name; }
  const std::string& Guid(void) const { return guid; }
  int LCN(void) const { return lcn; }
  CArgusTV::ChannelType Type(void) const { return type; }
  int ID(void) const { return id; }
  const std::string& GuideChannelID(void) const { return guidechannelid; };

private:
  std::string name;
  std::string guid;
  std::string guidechannelid;
  CArgusTV::ChannelType type = CArgusTV::Television;
  int lcn = 0;
  int id = 0;
};
