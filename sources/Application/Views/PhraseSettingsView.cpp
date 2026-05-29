/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "PhraseSettingsView.h"
#include "Application/Model/Phrase.h"
#include "ViewData.h"
#include <Application/AppWindow.h>
#include <nanoprintf.h>

PhraseSettingsView::PhraseSettingsView(GUIWindow &w, ViewData *viewData)
    : ScreenView(w, viewData), lengthFocused_(true) {}

PhraseSettingsView::~PhraseSettingsView() {}

void PhraseSettingsView::Reset() { lengthFocused_ = true; }

void PhraseSettingsView::updateLength(int delta) {
  Phrase &phrase = viewData_->song_->phrase_;
  uchar length = phrase.GetLength(viewData_->currentPhrase_);
  int next = static_cast<int>(length) + delta;
  if (next < MIN_STEPS_PER_PHRASE) {
    next = MIN_STEPS_PER_PHRASE;
  }
  if (next > MAX_STEPS_PER_PHRASE) {
    next = MAX_STEPS_PER_PHRASE;
  }
  phrase.SetLength(viewData_->currentPhrase_, static_cast<uchar>(next));
  viewData_->ClampPhraseEditorCursor();
  isDirty_ = true;
}

void PhraseSettingsView::ProcessButtonMask(unsigned short mask, bool pressed) {
  if (!pressed) {
    return;
  }

  if (mask & EPBM_NAV) {
    if (mask & EPBM_DOWN) {
      ViewType vt = VT_PHRASE;
      ViewEvent ve(VET_SWITCH_VIEW, &vt);
      SetChanged();
      NotifyObservers(&ve);
    }
    if (mask & EPBM_UP) {
      ViewType vt = VT_GROOVE;
      ViewEvent ve(VET_SWITCH_VIEW, &vt);
      SetChanged();
      NotifyObservers(&ve);
    }
    return;
  }

  if (mask & EPBM_ENTER) {
    if (mask & EPBM_DOWN) {
      updateLength(-1);
    }
    if (mask & EPBM_UP) {
      updateLength(1);
    }
    if (mask == EPBM_ENTER) {
      lengthFocused_ = !lengthFocused_;
      isDirty_ = true;
    }
    return;
  }

  if (lengthFocused_) {
    if (mask & EPBM_DOWN) {
      updateLength(-1);
    }
    if (mask & EPBM_UP) {
      updateLength(1);
    }
    return;
  }

  if (mask & EPBM_DOWN) {
    ViewType vt = VT_PHRASE;
    ViewEvent ve(VET_SWITCH_VIEW, &vt);
    SetChanged();
    NotifyObservers(&ve);
  }
  if (mask & EPBM_UP) {
    ViewType vt = VT_GROOVE;
    ViewEvent ve(VET_SWITCH_VIEW, &vt);
    SetChanged();
    NotifyObservers(&ve);
  }
}

void PhraseSettingsView::DrawView() {
  Clear();

  GUITextProperties props;
  GUIPoint pos = GetTitlePosition();
  SetColor(CD_NORMAL);

  char title[SCREEN_WIDTH + 1];
  npf_snprintf(title, sizeof(title), "Phrase %2.2X Settings",
               viewData_->currentPhrase_);
  DrawString(pos._x, pos._y, title, props);

  GUIPoint anchor = GetAnchor();
  pos = anchor;

  uchar length = viewData_->song_->phrase_.GetLength(viewData_->currentPhrase_);
  char line[SCREEN_WIDTH + 1];
  props.invert_ = lengthFocused_;
  npf_snprintf(line, sizeof(line), "Length: %2u",
               static_cast<unsigned>(length));
  DrawString(pos._x, pos._y, line, props);

  drawMap();
  drawNotes();
}

void PhraseSettingsView::OnFocus() { lengthFocused_ = true; }
