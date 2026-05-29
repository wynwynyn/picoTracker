# Changelog

## [1.0.1] - [2026-05-29] Fix hardfault on MIDI song columns 1, 2, 3, 5, 6, 7.

### Fixed

- MIDI instruments no longer HardFault on note-off for song columns 2, 3, 4, 6, 7, and 8 on Advance. Per-channel `lastNotes_` buffers are padded and aligned so word stores into SDRAM-backed `InstrumentBank` stay 4-byte aligned.

### Added

- `docs/midi-sdram-unaligned-access.md`: write-up of the SDRAM unaligned access bug, cause, and fix options.

## [1.0.0] [2026-05-29] Configurable phrase length + Advance SDRAM memory refactor.

### Added

- Per-phrase step length (up to 64 steps) with persistence in project files.
- Phrase editor scroll for phrases longer than one screen.
- Phrase Settings view to edit phrase length.
- SDRAM linker sections and model storage on Advance for song, instrument bank, tables, and groove data.
- Advance as the default CMake build target; `ADV` flag documented.

### Changed

- Phrase storage and playback use `MAX_STEPS_PER_PHRASE` (64) instead of a fixed 16 steps.
- Phrase clipboard buffers enlarged to 64 steps.
- `Project` on Advance holds references into SDRAM-backed `ModelStorage` instead of embedded song/instrument bank members.

### Fixed

- Advance boot hang by deferring `Table` construction until after FMC init.
- Advance save/load and Phrase Settings navigation after the SDRAM refactor.
- Phrase Settings UP/DOWN adjusts phrase length instead of changing views.
- Phrase length field display.
