/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _PHRASE_H_
#define _PHRASE_H_

#include "Foundation/Types/Types.h"

#ifdef ADV
#define PHRASE_COUNT 0xFF
#define NO_MORE_PHRASE 0x100
#else
#define PHRASE_COUNT 0x80
#define NO_MORE_PHRASE 0x81
#endif

#define MIN_STEPS_PER_PHRASE 16
#define MAX_STEPS_PER_PHRASE 64
#define LEGACY_STEPS_PER_PHRASE 16

// Storage stride for phrase step arrays (active length may be less).
#define STEPS_PER_PHRASE MAX_STEPS_PER_PHRASE

class Phrase {
public:
  Phrase();
  ~Phrase();
  void Reset();
  unsigned short GetNext();
  bool IsUsed(uchar i) { return isUsed_[i]; };
  void SetUsed(uchar c);
  void ClearAllocation();

  static int GetStepOffset(uchar phraseIndex, int step);
  uchar GetLength(uchar phraseIndex) const;
  void SetLength(uchar phraseIndex, uchar length);

  uchar note_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  uchar instr_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  FourCC cmd1_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  ushort param1_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  FourCC cmd2_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  ushort param2_[PHRASE_COUNT * MAX_STEPS_PER_PHRASE];
  uchar length_[PHRASE_COUNT];

private:
  bool isUsed_[PHRASE_COUNT];
};

#endif
