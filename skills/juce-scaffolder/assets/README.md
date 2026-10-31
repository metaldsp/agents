<!--
SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
SPDX-License-Identifier: LicenseRef-Proprietary
-->

# <PROJECT_NAME>

<PROJECT_DESCRIPTION>

This JUCE audio-effect starter includes a versioned APVTS parameter, state persistence, and
a realtime-safe `processBlock()` skeleton. The audio path is a pass-through until DSP is added.

## Build and Test

Install CMake, Ninja, a C++23 compiler, and the platform SDK required by JUCE. From the project
root, configure, build, and test with the default Clang preset:

```sh
cmake --preset Clang
cmake --build --preset Clang
ctest --preset Clang
```

`CMakePresets.json` also defines GCC, Release, Visual Studio, and Xcode presets.

The first configure downloads JUCE and GoogleTest into the ignored `libs/` directory. Builds also
copy enabled plugin formats to the platform plugin directory for local DAW testing.

## Before Shipping

Review the generated CMake metadata, especially the company, bundle ID, plugin formats, and
four-character plugin code. The supplied code must remain unique across all plugins.

## Licensing

Project code is proprietary; see `LICENSE` and `LICENSES/LicenseRef-Proprietary.txt`.
CMake and configuration files use CC0-1.0. The bundled git-commit, juce-dsp, and juce-ui
skills use MIT.
SPDX headers and `REUSE.toml` record licensing for individual files, with license texts in
`LICENSES/`. Install the REUSE tool and run `reuse lint` from the project root to check coverage.

## Extend the Starter

Define new versioned parameters in `source/PluginParameters.h`. Cache their APVTS atomics in the
processor constructor, load them once per audio block, and add matching tests whenever DSP state
or behavior changes.
