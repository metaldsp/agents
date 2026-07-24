## Agent Rules

- For `source/*.{cpp,h}` and files in source subdirectories, apply `.agents/rules/dsp.md`.
- For `source/UI/*.{cpp,h}`, also apply `.agents/rules/ui.md`; its UI guidance takes precedence.

## Build Commands

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Audio-Thread Rules

- Treat `processBlock()` and all callees as realtime code: do not allocate, lock, perform I/O,
  or access UI objects there.
- Start `processBlock()` with `juce::ScopedNoDenormals`.
- Cache APVTS raw parameter atomics in the processor constructor and load them with
  `std::memory_order_relaxed` in the audio callback. Do not call `getRawParameterValue()` there.
- Allocate and prepare DSP objects in `prepareToPlay()`, then reset stateful processors in
  `releaseResources()`.
- Move audio-to-UI updates onto the message thread with `juce::Timer` or `juce::AsyncUpdater`.
