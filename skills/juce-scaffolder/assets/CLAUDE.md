<!--
SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
SPDX-License-Identifier: LicenseRef-Proprietary
-->

## About This Project

A JUCE audio plugin using CMake and C++23. It builds cross-platform for Standalone, VST3, AU,
and AUv3 formats.

## Build Commands

```sh
# Configure (run once, or after CMakeLists.txt changes)
cmake --preset Clang

# Build
cmake --build --preset Clang

# Run tests
ctest --preset Clang --verbose --output-on-failure

# Standalone (macOS)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1.app

# Standalone (Linux)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1

# Standalone (Windows)
./build/source/<PROJECT_NAME>_artefacts/Debug/Standalone/<PROJECT_NAME>_v1.exe
```

For a universal macOS binary, configure with `cmake --preset Clang -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`.
