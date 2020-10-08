/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010-2011 Marcel Groothuis, Fred Hoogduin
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "settings.h"

#include <kodi/AddonBase.h>
#include <unordered_map>

class cPVRClientArgusTV;

class CArgusTVAddon : public kodi::addon::CAddonBase
{
public:
  CArgusTVAddon() = default;

  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override;
  void DestroyInstance(int instanceType,
                       const std::string& instanceID,
                       KODI_HANDLE addonInstance) override;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;
  const CSettings& GetSettings() const { return m_settings; }

private:
  CSettings m_settings;
  std::unordered_map<std::string, cPVRClientArgusTV*> m_usedInstances;
};
