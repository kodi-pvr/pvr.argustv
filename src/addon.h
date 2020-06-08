#pragma once
/*
 *      Copyright (C) 2005-2010 Team XBMC
 *      http://www.xbmc.org
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

#include "settings.h"

#include <kodi/AddonBase.h>
#include <unordered_map>

class cPVRClientArgusTV;

class ATTRIBUTE_HIDDEN CArgusTVAddon : public kodi::addon::CAddonBase
{
public:
  CArgusTVAddon() = default;

  ADDON_STATUS CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, const std::string& version, KODI_HANDLE& addonInstance) override;
  void DestroyInstance(int instanceType, const std::string& instanceID, KODI_HANDLE addonInstance) override;

  ADDON_STATUS SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue) override;
  const CSettings& GetSettings() const { return m_settings; }

private:
  CSettings m_settings;
  std::unordered_map<std::string, cPVRClientArgusTV*> m_usedInstances;
};
