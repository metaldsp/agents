---
description: Rules for real-time audio DSP in JUCE.
paths:
  - "source/*.{cpp,h}"
globs:
  - "source/*.{cpp,h}"
alwaysApply: true
applyTo: "**/source/*.cpp,**/source/*.h"
---

# DSP Coding Rules

## Threading Model

- The audio thread runs `processBlock()` and must never block, allocate, lock, or perform I/O.
- The message thread owns UI callbacks, parameter listeners, and timers.
- Exchange simple values with atomics; use lock-free queues for larger message-to-audio transfers.
- Send audio-to-UI updates through `juce::AsyncUpdater` or `juce::Timer`, never directly.

## Realtime Safety

- First executable line of `processBlock()`: `juce::ScopedNoDenormals noDenormals;`.
- Do not allocate, resize containers, lock, do file/network I/O, or call
  `getRawParameterValue()` in `processBlock()` or its callees.
- Allocate and prepare DSP objects in `prepareToPlay()`, then reset stateful objects in
  `releaseResources()`.
- Cache APVTS raw parameter atomics in the constructor and load them with
  `std::memory_order_relaxed` once per block.
- Use actual buffer channel/sample counts rather than hardcoding stereo.

## JUCE and Ownership

- Prefer `juce::dsp` helpers when they simplify a processing chain.
- Keep serialization and editor behavior out of low-level DSP classes.
- Use direct members for always-present DSP objects and `std::unique_ptr` for optional,
  heavyweight, or polymorphic ones; never create or swap ownership on the audio thread.
- Keep parameter IDs, names, defaults, and version hints in `source/PluginParameters.h`.
