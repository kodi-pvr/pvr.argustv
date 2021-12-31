/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/AddonBase.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 49943
#define DEFAULT_RADIO true
#define DEFAULT_TIMEOUT 10
#define DEFAULT_USER "Guest"
#define DEFAULT_PASS ""
#define DEFAULT_TUNEDELAY 200
#define DEFAULT_USEFOLDER false

class CSettings
{
public:
  CSettings() = default;

  bool Load();
  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::addon::CSettingValue& settingValue);

  std::string BaseURL() const
  {
    return "http://" + m_szHostname + ":" + std::to_string(m_iPort) + "/";
  }

  const std::string& Hostname() const { return m_szHostname; }
  int Port() const { return m_iPort; }
  int ConnectTimeout() const { return m_iConnectTimeout; }
  bool RadioEnabled() const { return m_bRadioEnabled; }
  const std::string& User() const { return m_szUser; }
  const std::string& Pass() const { return m_szPass; }
  int TuneDelay() const { return m_iTuneDelay; }
  bool UseFolder() const { return m_bUseFolder; }

private:
  std::string m_szHostname = DEFAULT_HOST;
  int m_iPort = DEFAULT_PORT;
  int m_iConnectTimeout = DEFAULT_TIMEOUT;
  bool m_bRadioEnabled = DEFAULT_RADIO;
  std::string m_szUser = DEFAULT_USER;
  std::string m_szPass = DEFAULT_PASS;
  int m_iTuneDelay = DEFAULT_TUNEDELAY;
  bool m_bUseFolder = DEFAULT_USEFOLDER;
};
