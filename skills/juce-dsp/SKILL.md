---
name: juce-dsp
description: >
  Implement and review real-time audio DSP in JUCE plugins and applications.
  Use for audio callbacks, processor lifecycle, parameter synchronization,
  and DSP stages. UI layout and styling are outside this skill's scope.
---

# JUCE DSP

Apply these guidelines to real-time JUCE audio processing. Follow the host
project's naming, namespace, parameter organization, and ownership conventions.

## Threading Model

Separate real-time processing from message-thread work:

- The audio thread runs `processBlock()` and must never block, allocate, or
  lock.
- The message thread runs UI callbacks and message-thread timers and owns
  the `MessageManager`.
- Parameter listeners may run on the audio thread or another calling thread;
  keep their callbacks fast, non-blocking, and thread-safe.

Use atomics or JUCE audio parameters for simple cross-thread values. Use a
lock-free queue for larger message-to-audio transfers. Publish audio-to-UI data through atomics
or a preallocated queue consumed by a message-thread timer. Keep UI calls and
message posting out of a strictly real-time callback.

## Audio-Thread Restrictions

Do not perform these operations in `processBlock()` or anything it calls:

- Heap allocation, deletion, or container growth
- File, network, or other blocking I/O
- Locks, including mutexes and spinlocks
- `dynamic_cast` unless its cost has been measured
- `AudioProcessorValueTreeState::getRawParameterValue()`

Allocate in constructors or `prepareToPlay()`. Prefer fixed-size or
preallocated storage and deterministic, lock-free operations.

## Denormal Handling

The first executable line of `processBlock()` must be:

```cpp
juce::ScopedNoDenormals noDenormals;
```

## `processBlock()` Guidelines

- Add the denormal guard first, then ignore unused MIDI or clear unused output
  channels as needed.
- Load cached atomics once per block unless sample-accurate behavior is
  required.
- Build `juce::dsp::AudioBlock` and processing contexts once per block.
- Do not reconstruct DSP objects, recalculate heavy state, or change ownership.
- Use the actual channel and sample counts rather than assuming stereo.

## Parameter Access

Cache raw parameter atomics during construction, then load them with relaxed
ordering on the audio thread:

```cpp
// Constructor
gainParameter = parameters.getRawParameterValue("gain");

// processBlock()
const float gainDb = gainParameter->load(std::memory_order_relaxed);
```

Use listeners to publish parameter changes or flag pending updates. Keep
bounded, allocation-free updates on the audio thread when appropriate; perform
expensive rebuilds off-thread and transfer prepared state with explicit
synchronization and lifetime management. Never mutate active DSP state from
another thread. In parameter listeners, use the callback's `newValue`.

Keep parameter IDs, names, ranges, choices, and defaults in the project's
parameter definitions. Smooth continuous changes where needed to avoid clicks.

## Lifecycle and JUCE DSP

In `prepareToPlay()`, allocate buffers, prepare each DSP stage, and seed state
from cached parameters. Derive channel counts from the supported bus layout.

In `releaseResources()`, reset stateful DSP objects.

- Prefer suitable `juce::dsp` utilities such as `AudioBlock`, `ProcessSpec`,
  `Oversampling`, `Gain`, and `IIR`.
- Respect `isBusesLayoutSupported()`.
- Report latency changes outside the sample loop.
- Keep editor and state-serialization concerns out of low-level DSP classes.

## Ownership and Conventions

- Use direct members for always-present DSP objects with simple lifetimes.
- Use `std::unique_ptr` for optional, heavyweight, or polymorphic objects.
- Never create, destroy, or swap owning pointers on the audio thread.
- Prefer small, clearly named `[[nodiscard]]` helpers over monolithic
  processing functions.

## Adding a DSP Stage

1. Add the object with intentional ownership.
2. Prepare it in `prepareToPlay()`.
3. Reset it in `releaseResources()` if it holds state.
4. Feed it from cached parameters or listener-driven updates.
5. Process it without allocation, locks, or hidden side effects.

## JUCE References

- [Parameter listeners](https://docs.juce.com/master/classjuce_1_1AudioProcessorListener.html):
  synchronous callbacks may occur during audio processing.
- [AudioProcessorValueTreeState](https://docs.juce.com/develop/classjuce_1_1AudioProcessorValueTreeState.html):
  raw parameter access and the non-real-time `copyState()` / `replaceState()` APIs.
- [APVTS listener values](https://docs.juce.com/master/structjuce_1_1AudioProcessorValueTreeState_1_1Listener.html):
  use `newValue` in the change callback.
