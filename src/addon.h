/*
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
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

  ADDON_STATUS CreateInstance(const kodi::addon::IInstanceInfo& instance,
                              KODI_ADDON_INSTANCE_HDL& hdl) override;
  void DestroyInstance(const kodi::addon::IInstanceInfo& instance,
                       const KODI_ADDON_INSTANCE_HDL hdl) override;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::addon::CSettingValue& settingValue) override;
  const CSettings& GetSettings() const { return m_settings; }

private:
  CSettings m_settings;
  std::unordered_map<std::string, cPVRClientArgusTV*> m_usedInstances;
};
