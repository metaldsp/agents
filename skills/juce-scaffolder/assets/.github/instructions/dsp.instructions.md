---
description: "Rules for real-time audio DSP in JUCE: denormal handling, ScopedNoDenormals, audio-thread safety constraints, processBlock guidelines, ownership, and repo conventions."
applyTo: "source/**"
---

# DSP Coding Rules

Use these instructions when writing or reviewing real-time audio code in
`source/**`. Their purpose is to keep DSP code denormal-safe, real-time safe,
JUCE-friendly, and consistent with the ownership and naming conventions used in
this repository.

## Real-Time Audio Thread Rules

Inside `processBlock()` and any function it calls, these are forbidden:

- Heap allocation: `new`, `delete`, `malloc`, `free`, `std::make_unique`, any
  container resize
- Blocking I/O: file access, network, `sleep`
- Locks: mutexes, spinlocks — use `std::atomic` or lock-free structures instead
- `dynamic_cast` unless its cost is measured and acceptable
- `getRawParameterValue()` — it acquires an internal lock

Use `std::atomic`, preallocated storage, or lock-free handoff mechanisms to
share state with the message thread.

## Denormal Handling

Always declare `juce::ScopedNoDenormals noDenormals;` as the first executable
line in `processBlock()`.

```cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    // Real-time DSP work...
}
```

Do not assume the host handles denormals for us. Set the guard explicitly on
every audio callback.

## processBlock() Guidelines

`processBlock()` should stay short, predictable, and easy to scan.

- Put denormal protection first.
- Clear or ignore unused outputs/MIDI explicitly when needed.
- Read cached atomic parameters once per block unless sample-accurate behavior
  is required.
- Build `juce::dsp::AudioBlock` / `ProcessContextReplacing` wrappers once and
  pass them through the DSP chain.
- Keep control flow simple and comments honest about the stages that actually
  run.
- Never rebuild DSP objects, recalculate heavy assets, or reconfigure ownership
  in the callback.

When iterating samples manually, use the block's real channel and sample counts.
Do not hardcode stereo assumptions into new DSP paths.

## Parameter Access

Do not call `AudioProcessorValueTreeState::getRawParameterValue()` from
`processBlock()`. Cache raw parameter atomics during construction, then load
them on the audio thread.

```cpp
// Constructor
m_gainParameter = m_parameters.getRawParameterValue(Parameters::gainID);

// processBlock — correct
const float gainDb = m_gainParameter->load(std::memory_order_relaxed);

// processBlock — wrong
const auto* p = m_parameters.getRawParameterValue(Parameters::gainID);
```

Use `AudioProcessorValueTreeState::Listener::parameterChanged()` for control-rate
or deferred work such as:

- updating DSP state that is safe outside the audio callback
- resetting or swapping processing modes
- rebuilding coefficients
- triggering background work

Anything shared back to the audio thread must still be synchronized with
`std::atomic` or a lock-free structure.

## prepareToPlay() and releaseResources()

Allocate and prepare everything up front in `prepareToPlay()`.

- construct or initialize DSP objects
- size scratch buffers
- initialize oversampling or helper processors
- call `prepare(spec)` on every DSP stage
- seed DSP state from cached parameter values

Use `juce::dsp::ProcessSpec` and derive the channel count from the actual bus
layout or processor state. Do not hardcode `2` in new code unless the class is
truly fixed to stereo by design.

`releaseResources()` should release transient state safely and call `reset()` on
DSP objects that keep internal history.

## JUCE DSP Best Practices

- Prefer `juce::dsp` utilities (`AudioBlock`, `ProcessContextReplacing`,
  `ProcessSpec`, `Oversampling`, `Gain`, `IIR`, etc.) where they make the code
  clearer and safer.
- Call `prepare()` exactly once per playback configuration change, and call
  `reset()` when transport or resource lifecycle requires state to be cleared.
- Respect `isBusesLayoutSupported()` when adding DSP. New processing code must
  behave correctly for every supported bus layout.
- If a processor exposes latency changes, update latency outside the hot sample
  loop and keep host reporting accurate.
- Keep state serialization and editor concerns out of low-level DSP classes.

## Ownership and Lifetime

Use single ownership for DSP objects.

- Prefer direct value members for always-present DSP objects with simple
  lifetimes.
- Use `std::unique_ptr` for optional, heavyweight, polymorphic, or dynamically
  created DSP objects.
- Own long-lived DSP objects in the processor or the class responsible for the
  full processing lifetime.
- Do not create, destroy, or swap owning pointers on the audio thread.

Apply `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR` to processor, editor, and
other owning classes.

## Naming and Codebase Conventions

Match the conventions already used in this repository.

- Put plugin classes in `namespace AudioPlugin`.
- Prefix member variables with `m_`.
- Name cached APVTS atomics like `m_driveParameter`,
  `m_oversamplingModeParameter`, and so on.
- Keep parameter IDs, names, choices, and defaults centralized in
  `source/Parameters.h`.
- Prefer small helper functions with clear names over large monolithic
  `processBlock()` bodies.
- Use `[[nodiscard]]` on helpers where ignoring the result would be a mistake.

## Adding or Changing DSP

When adding a DSP stage or helper:

1. Add the member with the repo naming conventions.
2. Choose ownership intentionally: direct member or `std::unique_ptr`.
3. Prepare and initialize it in `prepareToPlay()`.
4. Reset it in `releaseResources()` if it holds internal state.
5. Feed it from cached parameters or listener-driven updates.
6. Process it in `processBlock()` without allocations, locks, or hidden
   side effects.

If a DSP stage is optional, gate it behind a boolean helper instead of
duplicating inline parameter logic at each call site.

```cpp
if (isGateEnabled())
    m_noiseGate->process(context);
```

Prefer implementing helpers like `isGateEnabled()` as small `inline` or
`[[nodiscard]]` members that load a cached atomic and return a clear boolean.

Keep documentation and comments aligned with the code that really exists.
