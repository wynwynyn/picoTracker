# Background

This is an experimental repo for the picoTracker Advance, containing features that are not in the official releases of the pT/pTA firmware. Some of them are niche or specific features for my own preferences, and won't necessarily be suitable for consumer releases.

The `experimental` branch is the main base for these features. It is behind the official `master` by quite a bit, and has diverged a lot. `experimental` probably won't have the latest *official* updates and bugfixes. I'll try and keep the `master` branch in this repo up to date at least.

If you somehow find this branch, compile it, and upload it to your pT, do so at your own risk/curiousity. Having said that, I'm happy to answer general questions about the extra features :) 

All credit goes Xiphonics and the [picoTracker](https://github.com/xiphonics/picotracker) project!

### Experimental features todo

Some features include:
- [x] Sending `MCC` commands without having to trigger a note
- [x] Sending `MPC` commands without having to trigger a note
- [x] Able to set phrase lengths longer than 16 steps (phrase options screen)
    - involved memory refactor - moving things to SDRAM instead
- [x] 8 extra channels (16 total on ADV; horizontal song scroll, mixer page flip)
- [ ] Double the amount of ticks per row
- [ ] Add timestretch command
- [ ] MIDI effects
- [ ] [options] key repeat delay
- [ ] [options] default phrase length
- [ ] [very experimental] Able to set table speed
- [x] [very experimental] Able to set table lengths
- [ ] [very experimental] Chain tables together (trigger `TBL` from tables)

These features will only be tested on pTA - support for pT has been pretty much dropped for this repo.

### Core changes

**SDRAM memory layout**
The four largest data structures - `Song`, `InstrumentBank`, `Table`, and `Groove` - are no longer placed in SRAM. On ADV builds they live in dedicated linker sections (`.SONG_DATA`, `.INSTRUMENT_BANK`, `.TABLE_DATA`, `.GROOVE_DATA`) mapped to SDRAM1, and are constructed via placement new after FMC initialisation. This frees up a significant chunk of internal SRAM and is what makes the larger phrase and table storage feasible. One side effect: SDRAM enforces stricter alignment than SRAM, which surfaced a pre-existing alignment bug in `MidiInstrument` where per-channel chord state (`etl::array<uint8_t, 5>`) was packed without padding, causing unaligned reads and HardFaults on note-off for most channels. This was fixed as part of the refactor.

**Phrase model**
Phrase step storage was expanded from 16 to 64 steps per phrase (`MAX_STEPS_PER_PHRASE = 64`). Each phrase now has its own stored length (16-64, default 16), which is persisted in project files. All internal step indexing was refactored to use a stride offset helper (`GetStepOffset`) rather than hardcoded `phrase * 16 + pos` arithmetic, so adding per-phrase variable length didn't require touching every call site. Existing projects load fine since the default length is 16.

**Table model**
Table step storage was expanded from 16 to 128 steps per table (`MAX_TABLE_STEPS = 128`). Each table has its own active length (16-128), persisted as a `LENGTH` attribute in save files with a legacy default of 16 for backward compatibility. A new Table Settings screen (accessible via NAV+DOWN from the table editor) allows the length to be adjusted per-table.

**16-channel song model**
`SONG_CHANNEL_COUNT` was bumped from 8 to 16, with `STREAM_MIX_BUS = 16` and `MAX_BUS_COUNT = 17`. The mix bus layout, audio mixer module count, song grid drawing, and mixer page are all derived from this constant with no separate feature ifdefs needed. The song view gains horizontal scrolling to fit the extra channels, and the mixer page flips between two banks of 8.

### Issues

Check out [issues](https://github.com/llozd/picoTracker/issues) for bugs that I've probably introduced! 

## Development

Head over to the [Developer Guide](docs/DEV.md)
