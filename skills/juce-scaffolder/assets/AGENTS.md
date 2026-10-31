<!--
SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
SPDX-License-Identifier: LicenseRef-Proprietary
-->

## Build Commands

```sh
cmake --preset Clang
cmake --build --preset Clang
ctest --preset Clang --verbose --output-on-failure

# Standalone (macOS)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1.app/Contents/MacOS/<PROJECT_NAME>_v1

# Standalone (Linux)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1

# Standalone (Windows)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1.exe
```

For a universal macOS binary, configure with
`cmake --preset Clang -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`.

## Audio-Thread Rules

- Treat `processBlock()` and all callees as realtime code: do not allocate, lock, perform I/O,
  or access UI objects there.
- Start `processBlock()` with `juce::ScopedNoDenormals`.
- Cache APVTS raw parameter atomics in the processor constructor and load them with
  `std::memory_order_relaxed` in the audio callback. Do not call `getRawParameterValue()` there.
- Allocate and prepare DSP objects in `prepareToPlay()`, then reset stateful processors in
  `releaseResources()`.
- Move audio-to-UI updates onto the message thread with `juce::Timer` or `juce::AsyncUpdater`.

## Licensing

Add SPDX copyright and license headers to new files: `LicenseRef-Proprietary` for project code
and `CC0-1.0` for CMake and configuration files. Preserve existing third-party licenses.
Record licensing for uncommentable files in `REUSE.toml`. Run `reuse lint` after adding files.
