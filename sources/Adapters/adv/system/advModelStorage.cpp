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
#include <new>

ADV_SECTION_SONG_DATA static char songBuf[sizeof(Song)];
ADV_SECTION_INSTRUMENT_BANK static char instrumentBankBuf[sizeof(InstrumentBank)];
ADV_SECTION_TABLE_DATA static Table tableStorage[TABLE_COUNT];
ADV_SECTION_TABLE_DATA static bool tableAllocation[TABLE_COUNT];
ADV_SECTION_GROOVE_DATA static unsigned char grooveData[MAX_GROOVES][16];

static bool modelStorageInitialized = false;

void ModelStorage_Init() {
  if (modelStorageInitialized) {
    return;
  }
  new (songBuf) Song();
  new (instrumentBankBuf) InstrumentBank();
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

Table *ModelStorage_GetTables() { return tableStorage; }

bool *ModelStorage_GetTableAllocation() { return tableAllocation; }

unsigned char (*ModelStorage_GetGrooveData())[16] { return grooveData; }
