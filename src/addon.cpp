/*
 *      Copyright (C) 2010-2011 Marcel Groothuis, Fred Hoogduin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "addon.h"
#include "pvrclient-argustv.h"

ADDON_STATUS CArgusTVAddon::CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, const std::string& version, KODI_HANDLE& addonInstance)
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

void CArgusTVAddon::DestroyInstance(int instanceType, const std::string& instanceID, KODI_HANDLE addonInstance)
{
  const auto& it = m_usedInstances.find(instanceID);
  if (it != m_usedInstances.end())
  {
    it->second->Disconnect();
    m_usedInstances.erase(it);
  }
}

ADDON_STATUS CArgusTVAddon::SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue)
{
  return m_settings.SetSetting(settingName, settingValue);
}

ADDONCREATOR(CArgusTVAddon)
