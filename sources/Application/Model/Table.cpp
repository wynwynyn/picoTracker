/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2018 Discodirt
 * Copyright (c) 2024 xiphonics, inc.
 *
 * This file is part of the picoTracker firmware
 */

#include "Table.h"
#ifdef ADV
#include "ModelStorage.h"
#endif
#include "Application/Instruments/CommandList.h"
#include "Application/Utils/HexBuffers.h"
#include "Application/Utils/char.h"
#include "Song.h"
#include "System/System/System.h"

Table::Table() { Reset(); };

void Table::Reset() {
  length_ = LEGACY_TABLE_STEPS;
  for (int i = 0; i < TABLE_STEPS; i++) {
    cmd1_[i] = FourCC::InstrumentCommandNone;
    param1_[i] = 0;
    cmd2_[i] = FourCC::InstrumentCommandNone;
    param2_[i] = 0;
    cmd3_[i] = FourCC::InstrumentCommandNone;
    param3_[i] = 0;
  }
};

uchar Table::GetLength() const {
  uchar len = length_;
  if (len < MIN_TABLE_STEPS) {
    return MIN_TABLE_STEPS;
  }
  if (len > MAX_TABLE_STEPS) {
    return MAX_TABLE_STEPS;
  }
  return len;
}

void Table::SetLength(uchar length) {
  if (length < MIN_TABLE_STEPS) {
    length = MIN_TABLE_STEPS;
  }
  if (length > MAX_TABLE_STEPS) {
    length = MAX_TABLE_STEPS;
  }
  length_ = length;
}

void Table::Copy(const Table &other) {
  length_ = other.length_;
  for (int i = 0; i < TABLE_STEPS; i++) {
    cmd1_[i] = *(other.cmd1_ + i);
    param1_[i] = *(other.param1_ + i);
    cmd2_[i] = *(other.cmd2_ + i);
    param2_[i] = *(other.param2_ + i);
    cmd3_[i] = *(other.cmd3_ + i);
    param3_[i] = *(other.param3_ + i);
  }
};

bool Table::IsEmpty() {

  for (int i = 0; i < TABLE_STEPS; i++) {
    if (cmd1_[i] != FourCC::InstrumentCommandNone) {
      return false;
    };
    if (cmd2_[i] != FourCC::InstrumentCommandNone) {
      return false;
    };
    if (cmd3_[i] != FourCC::InstrumentCommandNone) {
      return false;
    };
    if (param1_[i] != 0) {
      return false;
    };
    if (param2_[i] != 0) {
      return false;
    };
    if (param3_[i] != 0) {
      return false;
    };
  }
  return true;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#ifdef ADV
Table *TableHolder::tables() { return ModelStorage_GetTables(); }

bool *TableHolder::allocations() { return ModelStorage_GetTableAllocation(); }
#else
Table *TableHolder::tables() { return table_; }

bool *TableHolder::allocations() { return allocation_; }
#endif

TableHolder::TableHolder() : Persistent("TABLES") { Reset(); }

void TableHolder::Reset() {
  for (int i = 0; i < TABLE_COUNT; i++) {
    tables()[i].Reset();
    allocations()[i] = false;
  }
};

Table &TableHolder::GetTable(int table) {
  NAssert((table >= 0) && (table < TABLE_COUNT));
  return tables()[table];
}

void TableHolder::SaveContent(tinyxml2::XMLPrinter *printer) {

  char hex[3];
  for (int i = 0; i < TABLE_COUNT; i++) {
    printer->OpenElement("TABLE");
    hex2char(i, hex);
    printer->PushAttribute("ID", hex);

    Table &table = tables()[i];
    if (!table.IsEmpty()) {
#ifdef ADV
      hex2char(table.GetLength(), hex);
      printer->PushAttribute("LENGTH", hex);
#endif
      saveHexBuffer(printer, "CMD1", table.cmd1_, TABLE_STEPS);
      saveHexBuffer(printer, "PARAM1", table.param1_, TABLE_STEPS);
      saveHexBuffer(printer, "CMD2", table.cmd2_, TABLE_STEPS);
      saveHexBuffer(printer, "PARAM2", table.param2_, TABLE_STEPS);
      saveHexBuffer(printer, "CMD3", table.cmd3_, TABLE_STEPS);
      saveHexBuffer(printer, "PARAM3", table.param3_, TABLE_STEPS);
    }
    printer->CloseElement();
  }
};

void TableHolder::RestoreContent(PersistencyDocument *doc) {

  bool elem = doc->FirstChild();
  while (elem) {
    // Check it is a table
    if (!strcmp(doc->ElemName(), "TABLE")) {
      unsigned char id = '\0';
      uchar length = LEGACY_TABLE_STEPS;
      bool attr = doc->NextAttribute();
      while (attr) {
        if (!strcmp(doc->attrname_, "ID")) {
          unsigned char b1 = (c2h__(doc->attrval_[0])) << 4;
          unsigned char b2 = c2h__(doc->attrval_[1]);
          id = b1 + b2;
        }
#ifdef ADV
        if (!strcmp(doc->attrname_, "LENGTH")) {
          unsigned char b1 = (c2h__(doc->attrval_[0])) << 4;
          unsigned char b2 = c2h__(doc->attrval_[1]);
          length = b1 + b2;
        }
#endif
        attr = doc->NextAttribute();
      }

      Table &table = tables()[id];
      table.Reset();
#ifdef ADV
      table.SetLength(length);
#endif

      bool subelem = doc->FirstChild();
      while (subelem) {
        if (!strcmp("CMD1", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.cmd1_);
        };
        if (!strcmp("PARAM1", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.param1_);
        };
        if (!strcmp("CMD2", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.cmd2_);
        };
        if (!strcmp("PARAM2", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.param2_);
        };
        if (!strcmp("CMD3", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.cmd3_);
        };
        if (!strcmp("PARAM3", doc->ElemName())) {
          restoreHexBuffer(doc, (unsigned char *)table.param3_);
        };
        subelem = doc->NextSibling();
      }
      allocations()[id] = !table.IsEmpty();
    }
    elem = doc->NextSibling();
  }
}

void TableHolder::SetUsed(int i) {
  if (i >= TABLE_COUNT) {
    NAssert(i < 128);
  }
  allocations()[i] = true;
};

int TableHolder::GetNext() {
  for (int i = 0; i < TABLE_COUNT; i++) {
    if (!allocations()[i]) {
      if (tables()[i].IsEmpty()) {
        allocations()[i] = true;
        return i;
      }
    };
  };
  return NO_MORE_TABLE;
};

int TableHolder::Clone(int table) {
  int target = GetNext();
  if (target != NO_MORE_TABLE) {
    tables()[target].Copy(tables()[table]);
  };
  return target;
};
