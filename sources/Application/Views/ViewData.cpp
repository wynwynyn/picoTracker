/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "ViewData.h"
#include "Application/Model/Table.h"
#include "BaseClasses/View.h"

ViewData::ViewData(Project *project) { Load(project); };

ViewData::~ViewData() {}

void ViewData::Load(Project *project) {
  project_ = project;
  song_ = &(project->song_);
  currentChain_ = 0;
  currentPhrase_ = 0;
  songX_ = 0;
  songY_ = 0;
  songOffset_ = 0;
  songChannelOffset_ = 0;
  mixerChannelOffset_ = 0;
  chainCol_ = 0;
  chainRow_ = 0;
  currentTable_ = 0;
  currentInstrumentID_ = 0;
  currentGroove_ = 0;
  playMode_ = PM_SONG;
  phraseOffset_ = 0;
  phraseCurPos_ = 0;
  tableOffset_ = 0;
  tableCurPos_ = 0;

  for (int i = 0; i < SONG_CHANNEL_COUNT; i++) {
    songPlayPos_[i] = 0;
    chainPlayPos_[i] = 0;
    phrasePlayPos_[i] = 0;
    currentPlayChain_[i] = 0xFF;
    currentPlayPhrase_[i] = 0xFF;
  }

  sampleEditorFilename.clear();
  isShowingSampleEditorProjectPool = false;
  importViewStartDir = nullptr;
}

unsigned char ViewData::UpdateSongChain(int offset) {
  unsigned char *c =
      song_->data_ + songX_ + SONG_CHANNEL_COUNT * (songOffset_ + songY_);
  updateData(c, offset, CHAIN_COUNT - 1, false);
  return *c;
}

void ViewData::SetSongChain(unsigned char value) {
  unsigned char *c =
      song_->data_ + songX_ + SONG_CHANNEL_COUNT * (songOffset_ + songY_);
  *c = value;
}

void ViewData::UpdateSongOffset(int offset) {
  songOffset_ += offset;
  checkSongBoundaries();
}

void ViewData::UpdateSongCursor(int dx, int dy) {
  songX_ += dx;
  songY_ += dy;
  checkSongBoundaries();
}

void ViewData::ClampSongEditorCursor() { checkSongBoundaries(); }

void ViewData::checkSongBoundaries() {
  int visibleCol = songX_ - songChannelOffset_;
  const int visibleCols = SONG_VISIBLE_COL_COUNT;

  if (visibleCol < 0) {
    songChannelOffset_ += visibleCol;
    visibleCol = 0;
  }
  if (visibleCol > visibleCols - 1) {
    songChannelOffset_ += visibleCol - visibleCols + 1;
    visibleCol = visibleCols - 1;
  }

  const int maxChannelOffset = SONG_CHANNEL_COUNT - visibleCols;
  if (songChannelOffset_ > maxChannelOffset) {
    songChannelOffset_ = maxChannelOffset;
  }
  if (songChannelOffset_ < 0) {
    songChannelOffset_ = 0;
  }

  if (visibleCol >= SONG_CHANNEL_COUNT - songChannelOffset_) {
    visibleCol = SONG_CHANNEL_COUNT - songChannelOffset_ - 1;
  }
  if (visibleCol < 0) {
    visibleCol = 0;
  }

  songX_ = songChannelOffset_ + visibleCol;

  if (songY_ < 0) {
    songOffset_ += songY_;
    songY_ = 0;
  };
  if (songY_ > View::songRowCount_ - 1) {
    songOffset_ += songY_ - View::songRowCount_ + 1;
    songY_ = View::songRowCount_ - 1;
  };
  if (songOffset_ > SONG_ROW_COUNT - View::songRowCount_) {
    songOffset_ = SONG_ROW_COUNT - View::songRowCount_;
  }
  if (songOffset_ < 0) {
    songOffset_ = 0;
  }
}

unsigned char *ViewData::GetCurrentSongPointer() {
  return song_->data_ + songX_ + SONG_CHANNEL_COUNT * (songOffset_ + songY_);
};

unsigned char ViewData::UpdateChainCursorValue(int offset, int dx, int dy) {

  unsigned char *c = 0;
  unsigned char limit = 0;
  bool wrap = false;

  switch (chainCol_ + dx) {
  case 0:
    c = song_->chain_.data_ + (16 * currentChain_ + chainRow_ + dy);
    limit = PHRASE_COUNT - 1;
    wrap = false;
    break;
  case 1:
    c = song_->chain_.transpose_ + (16 * currentChain_ + chainRow_ + dy);
    limit = 0xFF;
    wrap = true;
    break;
  }
  updateData(c, offset, limit, wrap);
  return *c;
}

void ViewData::UpdateChainCursor(int dx, int dy) {
  chainCol_ += dx;
  chainRow_ += dy;
  if (chainCol_ > 1)
    chainCol_ = 1;
  if (chainCol_ < 0)
    chainCol_ = 0;
  if (chainRow_ > 15)
    chainRow_ = 15;
  if (chainRow_ < 0)
    chainRow_ = 0;
}

void ViewData::SetChainPhrase(unsigned char value) {
  unsigned char *c = song_->chain_.data_ + (16 * currentChain_ + chainRow_);
  *c = value;
}

unsigned char *ViewData::GetCurrentChainPointer() {
  return song_->chain_.data_ + (16 * currentChain_ + chainRow_);
};

void ViewData::UpdatePhraseOffset(int offset) {
  phraseOffset_ += offset;
  int visibleRow = phraseCurPos_ - phraseOffset_;
  checkPhraseBoundaries(visibleRow);
  phraseCurPos_ = phraseOffset_ + visibleRow;
}

void ViewData::UpdatePhraseRow(int &visibleRow, int dy) {
  visibleRow += dy;
  checkPhraseBoundaries(visibleRow);
  phraseCurPos_ = phraseOffset_ + visibleRow;
}

int ViewData::GetAbsolutePhraseStep(int visibleRow) const {
  return phraseOffset_ + visibleRow;
}

void ViewData::ClampPhraseEditorCursor() {
  int visibleRow = phraseCurPos_ - phraseOffset_;
  checkPhraseBoundaries(visibleRow);
  phraseCurPos_ = phraseOffset_ + visibleRow;
}

void ViewData::checkPhraseBoundaries(int &visibleRow) {
  const int visibleRows = View::songRowCount_;
  const int len = song_->phrase_.GetLength(currentPhrase_);

  if (visibleRow < 0) {
    phraseOffset_ += visibleRow;
    visibleRow = 0;
  }
  if (visibleRow > visibleRows - 1) {
    phraseOffset_ += visibleRow - visibleRows + 1;
    visibleRow = visibleRows - 1;
  }

  const int maxOffset = len - visibleRows;
  if (phraseOffset_ > maxOffset) {
    phraseOffset_ = maxOffset;
  }
  if (phraseOffset_ < 0) {
    phraseOffset_ = 0;
  }

  if (visibleRow >= len - phraseOffset_) {
    visibleRow = len - phraseOffset_ - 1;
  }
  if (visibleRow < 0) {
    visibleRow = 0;
  }
}

void ViewData::UpdateTableRow(int &visibleRow, int dy) {
  visibleRow += dy;
  checkTableBoundaries(visibleRow);
  tableCurPos_ = tableOffset_ + visibleRow;
}

int ViewData::GetAbsoluteTableStep(int visibleRow) const {
  return tableOffset_ + visibleRow;
}

void ViewData::ClampTableEditorCursor() {
  int visibleRow = tableCurPos_ - tableOffset_;
  checkTableBoundaries(visibleRow);
  tableCurPos_ = tableOffset_ + visibleRow;
}

void ViewData::checkTableBoundaries(int &visibleRow) {
  const int visibleRows = View::songRowCount_;
  const int len =
      TableHolder::GetInstance()->GetTable(currentTable_).GetLength();

  if (visibleRow < 0) {
    tableOffset_ += visibleRow;
    visibleRow = 0;
  }
  if (visibleRow > visibleRows - 1) {
    tableOffset_ += visibleRow - visibleRows + 1;
    visibleRow = visibleRows - 1;
  }

  const int maxOffset = len - visibleRows;
  if (tableOffset_ > maxOffset) {
    tableOffset_ = maxOffset;
  }
  if (tableOffset_ < 0) {
    tableOffset_ = 0;
  }

  if (visibleRow >= len - tableOffset_) {
    visibleRow = len - tableOffset_ - 1;
  }
  if (visibleRow < 0) {
    visibleRow = 0;
  }
}
