/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010-2011 Marcel Groothuis, Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"

#include "pvrclient-argustv.h"

ADDON_STATUS CArgusTVAddon::CreateInstance(int instanceType,
                                           const std::string& instanceID,
                                           KODI_HANDLE instance,
                                           const std::string& version,
                                           KODI_HANDLE& addonInstance)
{
  ADDON_STATUS curStatus = ADDON_STATUS_UNKNOWN;

  if (instanceType == ADDON_INSTANCE_PVR)
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Creating octonet pvr instance", __func__);

    m_settings.Load();

    /* Connect to ARGUS TV */
    cPVRClientArgusTV* client = new cPVRClientArgusTV(*this, instance, version);
    if (!client->Connect())
    {
      curStatus = ADDON_STATUS_LOST_CONNECTION;
    }
    else
    {
      curStatus = ADDON_STATUS_OK;
    }

    addonInstance = client;
    m_usedInstances.emplace(std::make_pair(instanceID, client));
  }

  return curStatus;
}

void CArgusTVAddon::DestroyInstance(int instanceType,
                                    const std::string& instanceID,
                                    KODI_HANDLE addonInstance)
{
  const auto& it = m_usedInstances.find(instanceID);
  if (it != m_usedInstances.end())
  {
    it->second->Disconnect();
    m_usedInstances.erase(it);
  }
}

ADDON_STATUS CArgusTVAddon::SetSetting(const std::string& settingName,
                                       const kodi::CSettingValue& settingValue)
{
  return m_settings.SetSetting(settingName, settingValue);
}

#pragma GCC visibility push(default) // Temp workaround, this becomes later added to kodi-dev-kit system
ADDONCREATOR(CArgusTVAddon)
#pragma GCC visibility pop

