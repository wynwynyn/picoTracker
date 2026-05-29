/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _MODEL_STORAGE_H_
#define _MODEL_STORAGE_H_

#ifdef ADV

class Song;
class InstrumentBank;
class Table;
class Groove;

void ModelStorage_Init();

Song &ModelStorage_GetSong();
InstrumentBank &ModelStorage_GetInstrumentBank();
Table *ModelStorage_GetTables();
bool *ModelStorage_GetTableAllocation();
unsigned char (*ModelStorage_GetGrooveData())[16];

#endif

#endif
