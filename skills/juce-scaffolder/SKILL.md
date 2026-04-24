---
name: juce-scaffolder
description: Create a new JUCE audio plugin project in the current workspace from the bundled assets template. Use when the user asks to scaffold, generate, or create a JUCE plugin.
argument-hint: "[projectName] [projectDescription]"
metadata:
  author: Pier Luigi Fiorini
  license: MIT
  version: "1.0"
allowed-tools: Bash Read Write
inputs:
  - id: projectName
    type: promptString
    label: Project name
    description: CMake project name and replacement for <PROJECT_NAME>.
    required: true
  - id: projectDescription
    type: promptString
    label: Project description
    description: README description and replacement for <PROJECT_DESCRIPTION>.
    required: true
---

# JUCE Scaffolder

Create a JUCE plugin project in the current workspace from bundled `assets/`.

## Inputs

- `projectName`: required; replaces `<PROJECT_NAME>` and is the CMake project name.
- `projectDescription`: required; replaces `<PROJECT_DESCRIPTION>` in `README.md`.

If either value is missing, ask for it. Do not guess.

## Workflow

1. Treat the current workspace as the target directory.
2. If any generated path already exists, stop and report conflicts unless overwrite was explicitly requested.
3. Copy all files from bundled `assets/`, including dotfiles, preserving layout.
4. In generated text files, replace `<PROJECT_NAME>` and `<PROJECT_DESCRIPTION>`.
5. Update `LICENSE.md` and generated `.cpp`/`.h` copyright years to the current year.
6. Make no other template edits unless requested.
