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

#include "argustvrpc.h"

#include <json/json.h>
#include <string>

class cChannel
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

