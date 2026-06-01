/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2026
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _MIDI_DELAY_ENGINE_H_
#define _MIDI_DELAY_ENGINE_H_

#include "Foundation/Types/Types.h"
#include <stdint.h>

#define MAX_MIDI_DELAY_VOICES 32

class MidiDelayEngine {
public:
  static MidiDelayEngine &GetInstance();

  void SpawnChain(int songChannel, int midiChannel, uint8_t note,
                  uint8_t velocity, uint8_t repeatCount, uint8_t intervalTicks,
                  int8_t semitoneStep, uint8_t gateTicks);

  void AdvanceTick();
  void FlushChannel(int songChannel);
  void FlushAll();

  static int8_t ParseSignedTransposeByte(ushort param);

private:
  MidiDelayEngine() = default;

  struct Voice {
    bool active = false;
    uint8_t songChannel = 0;
    uint8_t midiChannel = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;
    uint8_t initialVelocity = 0;
    uint8_t repeatsLeft = 0;
    uint8_t totalRepeats = 0;
    uint8_t intervalTicks = 1;
    uint8_t ticksUntilNext = 0;
    int8_t semitoneStep = 0;
    uint8_t gateTicks = 1;
    uint8_t gateTicksRemaining = 0;
  };

  Voice *allocateVoice();
  void releaseVoice(Voice *voice);
  void sendNoteOn(const Voice &voice);
  void sendNoteOff(const Voice &voice);
  void advanceVoice(Voice &voice);
  void prepareNextEcho(Voice &voice);

  Voice voices_[MAX_MIDI_DELAY_VOICES];
};

#endif
