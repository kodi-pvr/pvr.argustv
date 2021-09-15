/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#if defined(TARGET_WINDOWS)
#pragma warning(disable : 4244) //wchar to char = loss of data
#endif

#include "utils.h"

#include "addon.h"

#include <algorithm> // sort
#include <kodi/General.h>
#include <string>

namespace Json
{
void printValueTree(const Json::Value& value, const std::string& path)
{
  switch (value.type())
  {
    case Json::nullValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=null\n", path.c_str());
      break;
    case Json::intValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=%d\n", path.c_str(), value.asInt());
      break;
    case Json::uintValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=%u\n", path.c_str(), value.asUInt());
      break;
    case Json::realValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=%.16g\n", path.c_str(), value.asDouble());
      break;
    case Json::stringValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=\"%s\"\n", path.c_str(), value.asString().c_str());
      break;
    case Json::booleanValue:
      kodi::Log(ADDON_LOG_DEBUG, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false");
      break;
    case Json::arrayValue:
    {
      kodi::Log(ADDON_LOG_DEBUG, "%s=[]\n", path.c_str());
      int size = value.size();
      for (int index = 0; index < size; ++index)
      {
        static char buffer[16];
        snprintf(buffer, 16, "[%d]", index);
        printValueTree(value[index], path + buffer);
      }
    }
    break;
    case Json::objectValue:
    {
      kodi::Log(ADDON_LOG_DEBUG, "%s={}\n", path.c_str());
      Json::Value::Members members(value.getMemberNames());
      std::sort(members.begin(), members.end());
      std::string suffix = *(path.end() - 1) == '.' ? "" : ".";
      for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
      {
        const std::string& name = *it;
        printValueTree(value[name], path + suffix + name);
      }
    }
    break;
    default:
      break;
  }
}
} //namespace Json

namespace BASE64
{

static const char* to_base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";

std::string b64_encode(unsigned char const* in, unsigned int in_len, bool urlEncode)
{
  std::string ret;
  int i(3);
  unsigned char c_3[3];
  unsigned char c_4[4];

  while (in_len)
  {
    i = in_len > 2 ? 3 : in_len;
    in_len -= i;
    c_3[0] = *(in++);
    c_3[1] = i > 1 ? *(in++) : 0;
    c_3[2] = i > 2 ? *(in++) : 0;

    c_4[0] = (c_3[0] & 0xfc) >> 2;
    c_4[1] = ((c_3[0] & 0x03) << 4) + ((c_3[1] & 0xf0) >> 4);
    c_4[2] = ((c_3[1] & 0x0f) << 2) + ((c_3[2] & 0xc0) >> 6);
    c_4[3] = c_3[2] & 0x3f;

    for (int j = 0; (j < i + 1); ++j)
    {
      if (urlEncode && to_base64[c_4[j]] == '+')
        ret += "%2B";
      else if (urlEncode && to_base64[c_4[j]] == '/')
        ret += "%2F";
      else
        ret += to_base64[c_4[j]];
    }
  }
  while ((i++ < 3))
    ret += urlEncode ? "%3D" : "=";
  return ret;
}

} //Namespace BASE64

// transform [\\nascat\qrecordings\NCIS\2012-05-15_20-30_SBS 6_NCIS.ts]
// into      [smb://user:password@nascat/qrecordings/NCIS/2012-05-15_20-30_SBS 6_NCIS.ts]
std::string ToCIFS(std::string& UNCName)
{
  std::string CIFSname = UNCName;
  std::string SMBPrefix = "smb://";
  size_t found;
  while ((found = CIFSname.find("\\")) != std::string::npos)
  {
    CIFSname.replace(found, 1, "/");
  }
  CIFSname.erase(0, 2);
  CIFSname.insert(0, SMBPrefix);
  return CIFSname;
}

bool InsertUser(const CArgusTVAddon& base, std::string& UNCName)
{
  if (base.GetSettings().User().empty())
    return false;

  if (UNCName.find("smb://") == 0)
  {
    std::string SMBPrefix = "smb://" + base.GetSettings().User();

    if (!base.GetSettings().Pass().empty())
      SMBPrefix.append(":" + base.GetSettings().Pass());

    SMBPrefix.append("@");

    UNCName.replace(0, std::string("smb://").length(), SMBPrefix);
    kodi::Log(ADDON_LOG_DEBUG, "Account Info added to SMB url");
    return true;
  }
  return false;
}


// transform [smb://user:password@nascat/qrecordings/NCIS/2012-05-15_20-30_SBS 6_NCIS.ts]
// into      [\\nascat\qrecordings\NCIS\2012-05-15_20-30_SBS 6_NCIS.ts]
std::string ToUNC(std::string& CIFSName)
{
  std::string UNCname = CIFSName;

  UNCname.erase(0, 6);
  size_t found;
  while ((found = UNCname.find("/")) != std::string::npos)
  {
    UNCname.replace(found, 1, "\\");
  }
  UNCname.insert(0, "\\\\");
  return UNCname;
}

std::string ToUNC(const char* CIFSName)
{
  std::string temp = CIFSName;
  return ToUNC(temp);
}

//////////////////////////////////////////////////////////////////////////////
