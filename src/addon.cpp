/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010-2011 Marcel Groothuis, Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"

#include "pvrclient-argustv.h"

ADDON_STATUS CArgusTVAddon::CreateInstance(const kodi::addon::IInstanceInfo& instance,
                                           KODI_ADDON_INSTANCE_HDL& hdl)
{
  ADDON_STATUS curStatus = ADDON_STATUS_UNKNOWN;

  if (instance.IsType(ADDON_INSTANCE_PVR))
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Creating octonet pvr instance", __func__);

    m_settings.Load();

    /* Connect to ARGUS TV */
    cPVRClientArgusTV* client = new cPVRClientArgusTV(*this, instance);
    if (!client->Connect())
    {
      curStatus = ADDON_STATUS_LOST_CONNECTION;
    }
    else
    {
      curStatus = ADDON_STATUS_OK;
    }

    hdl = client;
    m_usedInstances.emplace(std::make_pair(instance.GetID(), client));
  }

  return curStatus;
}

void CArgusTVAddon::DestroyInstance(const kodi::addon::IInstanceInfo& instance,
                                    const KODI_ADDON_INSTANCE_HDL hdl)
{
  const auto& it = m_usedInstances.find(instance.GetID());
  if (it != m_usedInstances.end())
  {
    it->second->Disconnect();
    m_usedInstances.erase(it);
  }
}

ADDON_STATUS CArgusTVAddon::SetSetting(const std::string& settingName,
                                       const kodi::addon::CSettingValue& settingValue)
{
  return m_settings.SetSetting(settingName, settingValue);
}

#pragma GCC visibility push( \
    default) // Temp workaround, this becomes later added to kodi-dev-kit system
ADDONCREATOR(CArgusTVAddon)
#pragma GCC visibility pop
