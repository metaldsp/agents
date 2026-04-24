# Repository Instructions

This repository is a JUCE audio plugin project generated from the `juce-scaffolder` skill.

- Use CMake as the source of truth for build structure.
- Keep plugin implementation files under `source/`.
- Keep tests under `test/` and use GoogleTest.
- Preserve the bundled `.clang-format` style.
- Prefer JUCE APIs and existing project patterns over introducing unrelated frameworks.
- Validate changes with `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build` when dependencies are available.
