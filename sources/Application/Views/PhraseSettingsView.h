/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _PHRASE_SETTINGS_VIEW_H_
#define _PHRASE_SETTINGS_VIEW_H_

#include "ScreenView.h"
#include "ViewData.h"

class PhraseSettingsView : public ScreenView {
public:
  PhraseSettingsView(GUIWindow &w, ViewData *viewData);
  ~PhraseSettingsView();
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
