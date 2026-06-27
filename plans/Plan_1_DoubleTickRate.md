# Plan 1: Double the Tick Rate

**Goal**: Change `AUDIO_SLICES_PER_STEP` from 6 to 12, so there are twice as many ticks per step while BPM remains the same.

## Background

The tick timing is governed by `SyncMaster` in `sources/Application/Player/SyncMaster.cpp`. The constant `AUDIO_SLICES_PER_STEP` (currently 6) defines how many "slices" (ticks) there are per 16th-note step. All timing formulas are derived from it:

```
playSampleCount_ = 60 * sampleRate * 2 / tempo / 8 / AUDIO_SLICES_PER_STEP
```

With 6 slices/step, PPQN = 6 × 4 = 24. With 12 slices/step, PPQN = 12 × 4 = 48. The BPM formula self-adjusts, so BPM is unchanged.

## Changes Required

### 1. `sources/Application/Player/SyncMaster.cpp:13-17`

The define appears **twice** inside a `#ifdef WIN32 / #else / #endif` block — both must be changed:

```cpp
// BEFORE:
#ifdef WIN32
#define AUDIO_SLICES_PER_STEP 6 // needs to be a multiple of 6 !
#else
#define AUDIO_SLICES_PER_STEP 6 // needs to be a multiple of 6 !
#endif

// AFTER:
#ifdef WIN32
#define AUDIO_SLICES_PER_STEP 12 // needs to be a multiple of 6 !
#else
#define AUDIO_SLICES_PER_STEP 12 // needs to be a multiple of 6 !
#endif
```

This halves `playSampleCount_` and `tickSampleCount_` — each slice is now half the real time, so slices run at 2× speed.

### 2. `sources/Application/Model/Groove.cpp:37-38`

```cpp
// BEFORE:
data[i][0] = 6;
data[i][1] = 6;

// AFTER:
data[i][0] = 12;
data[i][1] = 12;
```

**Why**: Groove values are in units of "ticks per step advance". With 6 slices/step, groove value 6 means the sequencer triggers once per step (at tick 0, then counts down 6→0, triggers again). With 12 slices/step, the same groove value of 6 would trigger twice per step (at tick 0 and tick 6). Changing defaults to 12 preserves the original straight-timing behavior.

**Backward compatibility note**: Any existing project files with saved groove patterns will play their grooves at double speed after this change. There is no migration mechanism.

## Verified Unchanged Behavior

### `SyncMaster::MidiSlice()` — MIDI clock output

```cpp
bool SyncMaster::MidiSlice() {
  int midiTick = currentSlice_ % (AUDIO_SLICES_PER_STEP / 6);
  return midiTick == 0;
}
```

- Before: `6 / 6 = 1` → fires every slice → 24 clocks/quarter (standard)
- After: `12 / 6 = 2` → fires every 2 slices → at 48 slices/quarter, still 24 clocks/quarter ✓

### `SyncMaster::TableSlice()` — per-tick table processing

```cpp
bool SyncMaster::TableSlice() {
  int tableTick = currentSlice_ % (AUDIO_SLICES_PER_STEP / 6 * tableRatio_);
  return tableTick == 0;
}
```

- Before: fires every slice (ratio=1)
- After: fires every 2 slices (ratio=1), but each slice is half the real time, so the absolute timing of table events is unchanged ✓

TableSlice is used in `Player.cpp:580` (time-to-live decrement) and `PlayerChannel.cpp:48` (passed as `updateTick` to instrument Render calls). Both are self-balancing because each slice is half the duration.

### `SyncMaster::MajorSlice()` — step boundary detection

```cpp
bool SyncMaster::MajorSlice() { return currentSlice_ == 0; };
```

Fires once per step regardless of slice count. ✓

### `SongView::OnPlayerUpdate()` — UI refresh throttling

```cpp
if ((eventType == PET_UPDATE) && (!sync->MajorSlice())) return;
```

Throttled by `MajorSlice()`. ✓

### `SyncMaster::GetTickTime()` — per-slice milliseconds

```cpp
return 60.0f * 2.0f / tempo_ / 8.0f / AUDIO_SLICES_PER_STEP * 1000.0f;
```

Returns ms per slice. With 12 slices, each slice is half the milliseconds. ✓

### `Player::Update()` — main sequencer loop

Called per audio buffer (not per slice). Still calls `sync->NextSlice()` once per call. ✓

## Files Changed (summary)

| File | Line(s) | Change |
|------|---------|--------|
| `sources/Application/Player/SyncMaster.cpp` | 14-16 | `AUDIO_SLICES_PER_STEP` 6 → 12 |
| `sources/Application/Model/Groove.cpp` | 37-38 | Default groove values 6 → 12 |
