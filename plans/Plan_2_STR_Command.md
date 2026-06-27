# Plan 2: New STR Command

**Command format**: `STR aabb` — handled by existing `SampleInstrument` (and optionally `MidiInstrument`).

- **AA** = advance amount — on each retrigger, the sample **start** position advances by AA samples
- **BB** = retrigger speed — number of ticks between retriggers (same semantics as RTG's `bb`)

**Example**: `STR 0101` → retriggers every tick, each time starting playback from a position 1 sample further into the sample.

**Key difference from RTG**: RTG adds an offset to the *current* playhead position (creating a forward-scrolling loop). STR resets the playhead to a *progressing absolute start* position (creating a scrubbing effect). Works correctly with sliced samples — the STR bounds are constrained to the slice boundaries.

---

## 1. FourCC Registration

### `sources/Foundation/Types/Types.h`

Add to the `FourCC` enum (pick an unused value — `53` is between RTG `52` and STOP `55`):

```cpp
InstrumentCommandSTR = 53,              // STR
```

Add to the ETL_ENUM_TYPE section:

```cpp
ETL_ENUM_TYPE(InstrumentCommandSTR, "STR")
```

The value `53` must be unique — verify no existing entry uses it. If `53` conflicts, use another unused value (available ranges: 1,3,5-19,21,23,24,28,32,33,35,37,40,41,43,47,49-51,53,54,56,57,59-61,63-65,67,68,70-91,93-142,144+).

---

## 2. Command List UI

### `sources/Application/Instruments/CommandList.cpp`

Add `FourCC::InstrumentCommandSTR` to the `_all[]` array. Insert it alphabetically under 'S' — place it near `InstrumentCommandStop`.

### `sources/Application/Utils/HelpLegend.h`

Add help text:

```cpp
case FourCC::InstrumentCommandSTR:
  result[0] = (char *)("STR: aabb");
  result[1] = (char *)("SAMPL:aa adv+bb speed, slices ok");
  break;
```

---

## 3. SampleInstrument: Render State Fields

### `sources/Application/Instruments/SampleRenderingParams.h`

Add six new fields to the `renderParams` struct (after the existing `retrigOffset_` at line 38, before `finished_` at line 40):

```cpp
bool str_;            // true if STR is active
int strAdvance_;      // AA value (samples to advance per retrigger)
int strSpeed_;        // BB value (ticks between retriggers)
int strCountdown_;    // ticks until next retrigger
int strCount_;        // total retriggers so far
int strAnchor_;       // original start position when STR was triggered
```

Total: 6 new fields (~18 bytes per channel). With 8-16 channels, that's ~144-288 bytes.

---

## 4. SampleInstrument: Start() Initialization

### `sources/Application/Instruments/SampleInstrument.cpp`

In `Start()`, in the `if (cleanstart)` block (line 549-581), add STR state reset alongside the existing retrigger reset (after line 556):

```cpp
rp->str_ = false;
rp->strAdvance_ = 0;
rp->strSpeed_ = 0;
rp->strCountdown_ = 0;
rp->strCount_ = 0;
rp->strAnchor_ = 0;
```

---

## 5. SampleInstrument: ProcessCommand()

### `sources/Application/Instruments/SampleInstrument.cpp`

In `SampleInstrument::ProcessCommand()`, add a new case alongside the existing `InstrumentCommandRetrigger` case (around line 1381-1394):

```cpp
case FourCC::InstrumentCommandSTR: {
  unsigned char advance = (value >> 8);   // AA
  unsigned char speed = (value & 0xFF);    // BB
  if (speed != 0) {
    rp->retrig_ = false;  // mutually exclusive with RTG
    rp->str_ = true;
    rp->strAdvance_ = advance;
    rp->strSpeed_ = speed;
    rp->strCountdown_ = speed;
    rp->strCount_ = 0;
    rp->strAnchor_ = rp->rendFirst_;  // capture the start position
  } else {
    rp->str_ = false;
  }
} break;
```

Note: `rendFirst_` is the slice start when sliced, so STR automatically anchors within the slice. ✓

Also update the RTG case to disable STR:

```cpp
case FourCC::InstrumentCommandRetrigger: {
  // ... existing code ...
  rp->str_ = false;  // add this
} break;
```

---

## 6. SampleInstrument: Render() Logic

### `sources/Application/Instruments/SampleInstrument.cpp`

In the render loop, the retrigger section is at lines 671-685. Add the STR logic after the existing RTG block (after line 685, before the closing brace of `if (updateTick)`):

```cpp
// Process STR
if (rp->str_) {
  if (rp->strCountdown_ == 0) {
    rp->strCount_++;
    int newPos = rp->strAnchor_ + rp->strAdvance_ * rp->strCount_;
    // Clamp to loop boundaries (respects slice bounds)
    if (newPos >= rp->rendLoopEnd_ || newPos < rp->rendLoopStart_) {
      newPos = rp->rendLoopStart_;   // wrap to start
      rp->strCount_ = 0;
    }
    rp->position_ = float(newPos);
    rp->strCountdown_ = rp->strSpeed_;
  }
  rp->strCountdown_--;
};
```

The clamping uses `rendLoopStart_`/`rendLoopEnd_`, which are already set to slice boundaries when a sliced sample is played. ✓

---

## 7. Persistence

No changes needed. STR is a phrase-step command like RTG, persisted in the existing pattern data format.

---

## Summary of Files Changed

| File | Change |
|------|--------|
| `sources/Foundation/Types/Types.h` | Add `InstrumentCommandSTR = 53` + `ETL_ENUM_TYPE("STR")` |
| `sources/Application/Instruments/CommandList.cpp` | Add `InstrumentCommandSTR` to command rotation |
| `sources/Application/Utils/HelpLegend.h` | Add help text for `STR` |
| `sources/Application/Instruments/SampleRenderingParams.h` | Add `str_`, `strAdvance_`, `strSpeed_`, `strCountdown_`, `strCount_`, `strAnchor_` fields |
| `sources/Application/Instruments/SampleInstrument.cpp` | Handle `InstrumentCommandSTR` in `ProcessCommand()`, add STR logic in `Render()`, reset STR state in `Start()` |

