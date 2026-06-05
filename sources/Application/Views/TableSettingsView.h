/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _TABLE_SETTINGS_VIEW_H_
#define _TABLE_SETTINGS_VIEW_H_

#include "ScreenView.h"
#include "ViewData.h"

class TableSettingsView : public ScreenView {
public:
  TableSettingsView(GUIWindow &w, ViewData *viewData);
  ~TableSettingsView();
  void Reset();
  virtual void ProcessButtonMask(unsigned short mask, bool pressed);
  virtual void DrawView();
  virtual void OnFocus();

protected:
  void updateLength(int delta);

private:
  bool lengthFocused_;
};

#endif
