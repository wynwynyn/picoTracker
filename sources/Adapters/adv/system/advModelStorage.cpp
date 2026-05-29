/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "Application/Model/ModelStorage.h"
#include "Application/Instruments/InstrumentBank.h"
#include "Application/Model/Groove.h"
#include "Application/Model/Song.h"
#include "Application/Model/Table.h"
#include "advMemorySections.h"
#include <cstring>
#include <new>

ADV_SECTION_SONG_DATA static char songBuf[sizeof(Song)];
ADV_SECTION_INSTRUMENT_BANK static char instrumentBankBuf[sizeof(InstrumentBank)];
ADV_SECTION_TABLE_DATA static char tableStorageBuf[sizeof(Table) * TABLE_COUNT];
ADV_SECTION_TABLE_DATA static bool tableAllocation[TABLE_COUNT];
ADV_SECTION_GROOVE_DATA static unsigned char grooveData[MAX_GROOVES][16];

static bool modelStorageInitialized = false;

void ModelStorage_Init() {
  if (modelStorageInitialized) {
    return;
  }
  std::memset(songBuf, 0, sizeof(songBuf));
  std::memset(instrumentBankBuf, 0, sizeof(instrumentBankBuf));
  std::memset(tableStorageBuf, 0, sizeof(tableStorageBuf));
  new (songBuf) Song();
  new (instrumentBankBuf) InstrumentBank();
  for (int i = 0; i < TABLE_COUNT; i++) {
    new (tableStorageBuf + i * sizeof(Table)) Table();
  }
  modelStorageInitialized = true;
}

Song &ModelStorage_GetSong() {
  ModelStorage_Init();
  return *reinterpret_cast<Song *>(songBuf);
}

InstrumentBank &ModelStorage_GetInstrumentBank() {
  ModelStorage_Init();
  return *reinterpret_cast<InstrumentBank *>(instrumentBankBuf);
}

Table *ModelStorage_GetTables() {
  ModelStorage_Init();
  return reinterpret_cast<Table *>(tableStorageBuf);
}

bool *ModelStorage_GetTableAllocation() { return tableAllocation; }

unsigned char (*ModelStorage_GetGrooveData())[16] { return grooveData; }
