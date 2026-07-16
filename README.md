## Background

This is an experimental repo for the picoTracker Advance, containing features that are not in the official releases of the pT/pTA firmware. Some of them are niche or specific features for my own preferences.

The `experimental` branch is the main base for these features. It is behind the official `master` by quite a bit, and has diverged a lot. `experimental` probably won't have the latest *official* updates and bugfixes. I'll try and keep the `master` branch in this repo up to date at least.

If you somehow find this branch, compile it, and upload it to your pT, do so at your own risk/curiousity. Having said that, I'm happy to answer general questions about the extra features :) 

All credit goes Xiphonics and the [picoTracker](https://github.com/xiphonics/picotracker) project!

### Experimental features TODO

- [x] Sending `MCC` commands without having to trigger a note
- [x] Sending `MPC` commands without having to trigger a note
- [ ] Sending table commands without having to trigger a note (enabling silent MCC/MPC control in tables)
- [x] Able to set phrase lengths longer than 16 steps (phrase options screen)
    - involved memory refactor - moving things to SDRAM instead
- [x] 8 extra channels (16 total on ADV; horizontal song scroll, mixer page flip)
- [x] Double the amount of ticks per row
- [x] Add timestretch command
- [ ] MIDI effects
- [ ] [options] key repeat delay
- [ ] [very experimental] Able to set table speed
- [x] [very experimental] Able to set table lengths

**These features will only be tested on pTA - support for pT has been pretty much dropped for this repo.**
