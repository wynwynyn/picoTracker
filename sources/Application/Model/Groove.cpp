/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "Groove.h"
#ifdef ADV
#include "ModelStorage.h"
#endif
#include <cstring>

#ifndef ADV
unsigned char Groove::data_[MAX_GROOVES][16];
#endif

unsigned char (*Groove::grooveRows())[16] {
#ifdef ADV
  return ModelStorage_GetGrooveData();
#else
  return data_;
#endif
}

Groove::Groove() : Persistent("GROOVES") { Clear(); };

Groove::~Groove(){};

void Groove::Clear() {
  unsigned char (*data)[16] = grooveRows();
  // Init all grooves with basic datas
  memset(data, NO_GROOVE_DATA, MAX_GROOVES * 0xF);
  for (int i = 0; i < MAX_GROOVES; i++) {
    data[i][0] = 6;
    data[i][1] = 6;
  };
  // init grooves selectah
  for (int i = 0; i < SONG_CHANNEL_COUNT; i++) {
    // Channel
    channelGroove_[i].groove_ = 0;
    channelGroove_[i].position_ = 0;
    channelGroove_[i].ticks_ = data[0][0];
  };
};
// Resest groove data at song startup

void Groove::Reset() {
  unsigned char (*data)[16] = grooveRows();
  for (int i = 0; i < SONG_CHANNEL_COUNT; i++) {
    // Channel
    ChannelGroove &c = channelGroove_[i];
    c.position_ = 0;
    c.ticks_ = data[c.groove_][c.position_];
  }
};

void Groove::GetChannelData(int channel, int *groove, int *position) {
  ChannelGroove &c = channelGroove_[channel];
  *groove = c.groove_;
  *position = c.position_;
};

void Groove::SaveContent(tinyxml2::XMLPrinter *printer) {
  saveHexBuffer(printer, "DATA", (unsigned char *)grooveRows(),
                16 * MAX_GROOVES);
};

void Groove::RestoreContent(PersistencyDocument *doc) {
  if (doc->FirstChild()) {
    restoreHexBuffer(doc, (unsigned char *)grooveRows());
  }
}

// Trigger grooves so we go to the next step
void Groove::Trigger() {
  for (int i = 0; i < SONG_CHANNEL_COUNT; i++) {
    ChannelGroove &c = channelGroove_[i];
    UpdateGroove(c, false);
  }
};

bool Groove::UpdateGroove(ChannelGroove &c, bool reverse) {

  bool stepped = false;
  unsigned char (*data)[16] = grooveRows();

  if (reverse) { // Table
    c.ticks_++;
    if (c.groove_ == 255) { // Default table groove
      if (c.ticks_ == 1) {
        stepped = true;
        c.ticks_ = 0;
      }
    } else {
      if (c.ticks_ == data[c.groove_][c.position_]) {
        c.position_ = (c.position_ + 1) % 16;
        if (data[c.groove_][c.position_] == 0xFF) {
          c.position_ = 0;
        };
        c.ticks_ = 0;
        stepped = true;
      }
    }
  } else { // Note
    if (c.ticks_ == 0) {
      c.position_ = (c.position_ + 1) % 16;
      if (data[c.groove_][c.position_] == 0xFF) {
        c.position_ = 0;
      };
      c.ticks_ = data[c.groove_][c.position_];
      stepped = true;
    };
    c.ticks_--;
  }
  return stepped;
}

void Groove::SetGroove(int channel, int groove) {
  if (groove >= MAX_GROOVES)
    return;
  unsigned char (*data)[16] = grooveRows();
  channelGroove_[channel].groove_ = groove;
  channelGroove_[channel].position_ = 0;
  channelGroove_[channel].ticks_ =
      data[channelGroove_[channel].groove_][channelGroove_[channel].position_];
};

// Returns true if, according to current groove setting it is time to go
// to the next sequencing step

bool Groove::TriggerChannel(int i) {
  unsigned char (*data)[16] = grooveRows();
  ChannelGroove &c = channelGroove_[i];
  return ((c.ticks_) % (data[c.groove_][c.position_]) == 0);
};

unsigned char *Groove::GetGrooveData(int groove) {
  return grooveRows()[groove];
};
