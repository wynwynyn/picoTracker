/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _VIEW_EVENT_H_
#define _VIEW_EVENT_H_

#include "Foundation/Observable.h"

enum ViewType {
  VT_SONG,
  VT_CHAIN,
  VT_PHRASE,
  VT_PHRASE_SETTINGS,
  VT_PROJECT,
  VT_DEVICE,
  VT_INSTRUMENT,
  VT_TABLE,  // Table screen under phrase
  VT_TABLE2, // Table screen under instrument
  VT_TABLE_SETTINGS,
  VT_GROOVE,
  VT_MIXER,
  VT_IMPORT,            // Sample file import
  VT_INSTRUMENT_IMPORT, // Instrument file import
  VT_SELECTPROJECT,     // Select project
  VT_THEME,             // Theme settings
  VT_SELECTTHEME,       // Theme selection
  VT_THEME_IMPORT,      // Theme file import
  VT_SAMPLE_EDITOR,     // Sample Editor
  VT_SAMPLE_SLICES,     // Sample slice editor
  VT_RECORD             // Recording screen
};

enum ViewEventType {
  VET_SWITCH_VIEW,
  VET_PLAYER_POSITION_UPDATE,
  VET_LIST_SELECT,
  VET_LOAD_PROJECT,
  VET_NEW_PROJECT,
  VET_QUIT_PROJECT,
  VET_UPDATE,
  VET_QUIT_APP
};

class ViewEvent : public I_ObservableData {
public:
  ViewEvent(ViewEventType type, void *data = 0);
  ViewEventType GetType();
  void *GetData();

private:
  ViewEventType type_;
  void *data_;
};

#endif
