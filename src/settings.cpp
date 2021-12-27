/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "settings.h"

bool CSettings::Load()
{

  /* Read ARGUS TV PVR client settings */
  //  See also addons/pvr.argustv/resources/settings.xml
  //  and addons/pvr.argustv/resources/language/.../strings.xml

  if (!kodi::addon::CheckSettingString("host", m_szHostname))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR,
              "Couldn't get 'host' setting, falling back to '127.0.0.1' as default");
    m_szHostname = DEFAULT_HOST;
  }

  /* Read setting "port" from settings.xml */
  if (!kodi::addon::CheckSettingInt("port", m_iPort))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR, "Couldn't get 'port' setting, falling back to '49943' as default");
    m_iPort = DEFAULT_PORT;
  }

  /* Read setting "useradio" from settings.xml */
  if (!kodi::addon::CheckSettingBoolean("useradio", m_bRadioEnabled))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR,
              "Couldn't get 'useradio' setting, falling back to 'true' as default");
    m_bRadioEnabled = DEFAULT_RADIO;
  }

  /* Read setting "timeout" from settings.xml */
  if (!kodi::addon::CheckSettingInt("timeout", m_iConnectTimeout))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR,
              "Couldn't get 'timeout' setting, falling back to %i seconds as default",
              DEFAULT_TIMEOUT);
    m_iConnectTimeout = DEFAULT_TIMEOUT;
  }

  /* read setting "user" from settings.xml */
  if (!kodi::addon::CheckSettingString("user", m_szUser))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR, "Couldn't get 'user' setting, falling back to '%s' as default",
              DEFAULT_USER);
    m_szUser = DEFAULT_USER;
  }

  /* read setting "pass" from settings.xml */
  if (!kodi::addon::CheckSettingString("pass", m_szPass))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR, "Couldn't get 'pass' setting, leaved empty");
    m_szPass = DEFAULT_USER;
  }

  /* Read setting "tunedelay" from settings.xml */
  if (!kodi::addon::CheckSettingInt("tunedelay", m_iTuneDelay))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR,
              "Couldn't get 'tunedelay' setting, falling back to '200' as default");
    m_iTuneDelay = DEFAULT_TUNEDELAY;
  }

  /* Read setting "usefolder" from settings.xml */
  if (!kodi::addon::CheckSettingBoolean("usefolder", m_bUseFolder))
  {
    /* If setting is unknown fallback to defaults */
    kodi::Log(ADDON_LOG_ERROR,
              "Couldn't get 'usefolder' setting, falling back to 'false' as default");
    m_bUseFolder = DEFAULT_USEFOLDER;
  }

  return true;
}

ADDON_STATUS CSettings::SetSetting(const std::string& settingName,
                                   const kodi::addon::CSettingValue& settingValue)
{
  if (settingName == "host")
  {
    std::string tmp_sHostname;
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'host' from %s to %s", m_szHostname.c_str(),
              settingValue.GetString().c_str());
    tmp_sHostname = m_szHostname;
    m_szHostname = settingValue.GetString();
    if (tmp_sHostname != m_szHostname)
      return ADDON_STATUS_NEED_RESTART;
  }
  else if (settingName == "port")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'port' from %u to %u", m_iPort,
              settingValue.GetInt());
    if (m_iPort != settingValue.GetInt())
    {
      m_iPort = settingValue.GetInt();
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "useradio")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed setting 'useradio' from %u to %u", m_bRadioEnabled,
              settingValue.GetBoolean());
    m_bRadioEnabled = settingValue.GetBoolean();
  }
  else if (settingName == "timeout")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed setting 'timeout' from %u to %u", m_iConnectTimeout,
              settingValue.GetInt());
    m_iConnectTimeout = settingValue.GetInt();
  }
  else if (settingName == "user")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'user' from %s to %s", m_szUser.c_str(),
              settingValue.GetString().c_str());
    m_szUser = settingValue.GetString();
  }
  else if (settingName == "pass")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'pass' from %s to %s", m_szPass.c_str(),
              settingValue.GetString().c_str());
    m_szPass = settingValue.GetString();
  }
  else if (settingName == "tunedelay")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed setting 'tunedelay' from %u to %u", m_iTuneDelay,
              settingValue.GetInt());
    m_iTuneDelay = settingValue.GetInt();
  }
  else if (settingName == "usefolder")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed setting 'usefolder' from %u to %u", m_bUseFolder,
              settingValue.GetBoolean());
    m_bUseFolder = settingValue.GetBoolean();
  }

  return ADDON_STATUS_OK;
}
