/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "Phrase.h"
#include "Song.h"
#include <stdlib.h>
#include <string.h>

Phrase::Phrase() { Reset(); };

Phrase::~Phrase(){};

void Phrase::Reset() {
  for (int i = 0; i < PHRASE_COUNT * MAX_STEPS_PER_PHRASE; i++) {
    note_[i] = NO_NOTE;
    instr_[i] = 0xFF;
    cmd1_[i] = FourCC::InstrumentCommandNone;
    param1_[i] = 0x00;
    cmd2_[i] = FourCC::InstrumentCommandNone;
    param2_[i] = 0x00;
  }
  for (int i = 0; i < PHRASE_COUNT; i++) {
    isUsed_[i] = false;
    length_[i] = MIN_STEPS_PER_PHRASE;
  }
}

int Phrase::GetStepOffset(uchar phraseIndex, int step) {
  return phraseIndex * MAX_STEPS_PER_PHRASE + step;
}

uchar Phrase::GetLength(uchar phraseIndex) const {
  if (phraseIndex >= PHRASE_COUNT) {
    return MIN_STEPS_PER_PHRASE;
  }
  uchar len = length_[phraseIndex];
  if (len < MIN_STEPS_PER_PHRASE) {
    return MIN_STEPS_PER_PHRASE;
  }
  if (len > MAX_STEPS_PER_PHRASE) {
    return MAX_STEPS_PER_PHRASE;
  }
  return len;
}

void Phrase::SetLength(uchar phraseIndex, uchar length) {
  if (phraseIndex >= PHRASE_COUNT) {
    return;
  }
  if (length < MIN_STEPS_PER_PHRASE) {
    length = MIN_STEPS_PER_PHRASE;
  }
  if (length > MAX_STEPS_PER_PHRASE) {
    length = MAX_STEPS_PER_PHRASE;
  }
  length_[phraseIndex] = length;
}

unsigned short Phrase::GetNext() {
  for (int i = 0; i < PHRASE_COUNT; i++) {
    if (!isUsed_[i]) {
      isUsed_[i] = true;
      return i;
    }
  }
  return NO_MORE_PHRASE;
};

void Phrase::SetUsed(unsigned char c) { isUsed_[c] = true; }

void Phrase::ClearAllocation() {

  for (int i = 0; i < PHRASE_COUNT; i++) {
    isUsed_[i] = false;
  }
};
