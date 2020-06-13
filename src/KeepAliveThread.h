/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Marcel Groothuis
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <p8-platform/threads/threads.h>

class cPVRClientArgusTV;

class CKeepAliveThread : public P8PLATFORM::CThread
{
public:
  CKeepAliveThread(cPVRClientArgusTV& instance);
  virtual ~CKeepAliveThread(void);

private:
  virtual void* Process(void);

  cPVRClientArgusTV& m_instance;
};
