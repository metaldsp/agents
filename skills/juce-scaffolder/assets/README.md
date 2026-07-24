# <PROJECT_NAME>

<PROJECT_DESCRIPTION>

This JUCE audio-effect starter includes a versioned APVTS gain parameter, realtime-safe
parameter access, state persistence, and a standard rotary-slider attachment.

## Build and Test

Install CMake, Ninja, a C++23 compiler, and the platform SDK required by JUCE. From the project
root, configure, build, and test:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

The first configure downloads JUCE and GoogleTest into the ignored `libs/` directory. Builds also
copy enabled plugin formats to the platform plugin directory for local DAW testing.

## Before Shipping

Review the generated CMake metadata, especially the company, bundle ID, plugin formats, and
four-character plugin code. The supplied code must remain unique across all plugins.

## Extend the Starter

Define new versioned parameters in `source/PluginParameters.h`. Cache their APVTS atomics in the
processor constructor, load them once per audio block, and add matching tests whenever DSP state
or behavior changes.
