/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2026
 *
 * This file is part of the picoTracker firmware
 */

#include "MidiDelayEngine.h"
#include "Services/Midi/MidiMessage.h"
#include "Services/Midi/MidiService.h"

namespace {

uint8_t clampNote(int note) {
  if (note < 0) {
    return 0;
  }
  if (note > 127) {
    return 127;
  }
  return static_cast<uint8_t>(note);
}

uint8_t echoVelocity(uint8_t initialVelocity, uint8_t repeatsLeft,
                    uint8_t totalRepeats) {
  if (totalRepeats == 0) {
    return 1;
  }
  unsigned int scaled =
      (static_cast<unsigned int>(initialVelocity) * repeatsLeft) / totalRepeats;
  if (scaled < 1) {
    return 1;
  }
  if (scaled > 127) {
    return 127;
  }
  return static_cast<uint8_t>(scaled);
}

} // namespace

MidiDelayEngine &MidiDelayEngine::GetInstance() {
  static MidiDelayEngine instance;
  return instance;
}

int8_t MidiDelayEngine::ParseSignedTransposeByte(ushort param) {
  int step = param & 0xFF;
  if (step > 127) {
    step -= 256;
  }
  return static_cast<int8_t>(step);
}

MidiDelayEngine::Voice *MidiDelayEngine::allocateVoice() {
  for (auto &voice : voices_) {
    if (!voice.active) {
      return &voice;
    }
  }
  return nullptr;
}

void MidiDelayEngine::releaseVoice(Voice *voice) {
  if (voice != nullptr) {
    voice->active = false;
    voice->gateTicksRemaining = 0;
    voice->ticksUntilNext = 0;
    voice->repeatsLeft = 0;
  }
}

void MidiDelayEngine::sendNoteOn(const Voice &voice) {
  MidiService *svc = MidiService::GetInstance();
  if (svc == nullptr) {
    return;
  }
  MidiMessage msg;
  msg.status_ = MidiMessage::MIDI_NOTE_ON + voice.midiChannel;
  msg.data1_ = voice.note;
  msg.data2_ = voice.velocity;
  svc->QueueMessage(msg);
}

void MidiDelayEngine::sendNoteOff(const Voice &voice) {
  MidiService *svc = MidiService::GetInstance();
  if (svc == nullptr) {
    return;
  }
  MidiMessage msg;
  msg.status_ = MidiMessage::MIDI_NOTE_OFF + voice.midiChannel;
  msg.data1_ = voice.note;
  msg.data2_ = 0x00;
  svc->QueueMessage(msg);
}

void MidiDelayEngine::prepareNextEcho(Voice &voice) {
  int nextNote = static_cast<int>(voice.note) + voice.semitoneStep;
  voice.note = clampNote(nextNote);
  voice.velocity =
      echoVelocity(voice.initialVelocity, voice.repeatsLeft, voice.totalRepeats);
}

void MidiDelayEngine::advanceVoice(Voice &voice) {
  if (!voice.active) {
    return;
  }

  if (voice.gateTicksRemaining > 0) {
    voice.gateTicksRemaining--;
    if (voice.gateTicksRemaining == 0) {
      sendNoteOff(voice);
      if (voice.repeatsLeft == 0) {
        releaseVoice(&voice);
        return;
      }
      voice.ticksUntilNext = voice.intervalTicks;
    }
    return;
  }

  if (voice.ticksUntilNext > 0) {
    voice.ticksUntilNext--;
    if (voice.ticksUntilNext > 0) {
      return;
    }
  }

  if (voice.repeatsLeft == 0) {
    releaseVoice(&voice);
    return;
  }

  sendNoteOn(voice);
  voice.repeatsLeft--;
  voice.gateTicksRemaining = voice.gateTicks;
  if (voice.repeatsLeft > 0) {
    prepareNextEcho(voice);
  }
}

void MidiDelayEngine::SpawnChain(int songChannel, int midiChannel, uint8_t note,
                                 uint8_t velocity, uint8_t repeatCount,
                                 uint8_t intervalTicks, int8_t semitoneStep,
                                 uint8_t gateTicks) {
  if (repeatCount == 0) {
    return;
  }

  Voice *voice = allocateVoice();
  if (voice == nullptr) {
    return;
  }

  if (intervalTicks == 0) {
    intervalTicks = 1;
  }
  if (gateTicks == 0) {
    gateTicks = 1;
  }

  voice->active = true;
  voice->songChannel = static_cast<uint8_t>(songChannel);
  voice->midiChannel = static_cast<uint8_t>(midiChannel);
  voice->note = note;
  voice->velocity = velocity;
  voice->initialVelocity = velocity;
  voice->repeatsLeft = repeatCount;
  voice->totalRepeats = repeatCount;
  voice->intervalTicks = intervalTicks;
  voice->ticksUntilNext = intervalTicks;
  voice->semitoneStep = semitoneStep;
  voice->gateTicks = gateTicks;
  voice->gateTicksRemaining = 0;
}

void MidiDelayEngine::AdvanceTick() {
  for (auto &voice : voices_) {
    if (voice.active) {
      advanceVoice(voice);
    }
  }
}

void MidiDelayEngine::FlushChannel(int songChannel) {
  for (auto &voice : voices_) {
    if (voice.active && voice.songChannel == static_cast<uint8_t>(songChannel)) {
      if (voice.gateTicksRemaining > 0) {
        sendNoteOff(voice);
      }
      releaseVoice(&voice);
    }
  }
}

void MidiDelayEngine::FlushAll() {
  for (auto &voice : voices_) {
    if (voice.active) {
      if (voice.gateTicksRemaining > 0) {
        sendNoteOff(voice);
      }
      releaseVoice(&voice);
    }
  }
}
