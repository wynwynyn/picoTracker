/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#ifndef _ADV_MEMORY_SECTIONS_H_
#define _ADV_MEMORY_SECTIONS_H_

#define ADV_SECTION_SONG_DATA                                                  \
  __attribute__((section(".SONG_DATA"), aligned(32)))
#define ADV_SECTION_INSTRUMENT_BANK                                            \
  __attribute__((section(".INSTRUMENT_BANK"), aligned(32)))
#define ADV_SECTION_TABLE_DATA                                                 \
  __attribute__((section(".TABLE_DATA"), aligned(32)))
#define ADV_SECTION_GROOVE_DATA                                                \
  __attribute__((section(".GROOVE_DATA"), aligned(32)))

#endif
