/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "TableSettingsView.h"
#include "Application/Model/Table.h"
#include "ViewData.h"
#include <Application/AppWindow.h>
#include <nanoprintf.h>

ViewType TableSettingsView::sourceViewType_ = VT_TABLE;

TableSettingsView::TableSettingsView(GUIWindow &w, ViewData *viewData)
    : ScreenView(w, viewData), lengthFocused_(false) {}

TableSettingsView::~TableSettingsView() {}

void TableSettingsView::Reset() { lengthFocused_ = false; }

void TableSettingsView::SetSourceViewType(ViewType vt) { sourceViewType_ = vt; }

void TableSettingsView::updateLength(int delta) {
  Table &table = TableHolder::GetInstance()->GetTable(viewData_->currentTable_);
  uchar length = table.GetLength();
  int next = static_cast<int>(length) + delta;
  if (next < MIN_TABLE_STEPS) {
    next = MIN_TABLE_STEPS;
  }
  if (next > MAX_TABLE_STEPS) {
    next = MAX_TABLE_STEPS;
  }
  table.SetLength(static_cast<uchar>(next));
  viewData_->ClampTableEditorCursor();
  isDirty_ = true;
}

void TableSettingsView::ProcessButtonMask(unsigned short mask, bool pressed) {
  if (!pressed) {
    return;
  }

  if (mask & EPBM_NAV) {
    if (mask & EPBM_UP) {
      ViewType vt = sourceViewType_;
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
  }
}

void TableSettingsView::DrawView() {
  Clear();

  GUITextProperties props;
  GUIPoint pos = GetTitlePosition();
  SetColor(CD_NORMAL);

  char title[SCREEN_WIDTH + 1];
  npf_snprintf(title, sizeof(title), "Table %2.2X Settings",
               viewData_->currentTable_);
  DrawString(pos._x, pos._y, title, props);

  GUIPoint anchor = GetAnchor();
  pos = anchor;

  uchar length = TableHolder::GetInstance()
                     ->GetTable(viewData_->currentTable_)
                     .GetLength();
  char line[SCREEN_WIDTH + 1];
  props.invert_ = lengthFocused_;
  npf_snprintf(line, sizeof(line), "Length: %3u",
               static_cast<unsigned>(length));
  DrawString(pos._x, pos._y, line, props);

  drawMap();
  drawNotes();
}

void TableSettingsView::OnFocus() {
  lengthFocused_ = true;
  isDirty_ = true;
}
