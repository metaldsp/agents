---
name: juce-scaffolder
description: Scaffold a new JUCE audio plugin project from a bundled CMake/JUCE template. Use when the user asks to create, generate, bootstrap, or scaffold a JUCE plugin, audio plugin, VST3/AU/Standalone plugin, or MetalDSP-style JUCE project in the current workspace.
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

Scaffold a JUCE plugin project in the current workspace from bundled `assets/`.

## Inputs

- `projectName`: required CMake project name; should be short and identifier-like, for example `MyPlugin`.
- `projectDescription`: required README description.

If either value is missing, ask for it. Do not guess.

## Workflow

1. Treat the current workspace as the target directory.
2. Resolve the skill directory from this `SKILL.md`.
3. Run the bundled scaffold script:

   ```bash
   python3 <skill-dir>/scripts/scaffold.py --target . --name "<projectName>" --description "<projectDescription>"
   ```

4. If the script reports path conflicts, stop and show the conflicting paths. Re-run with `--overwrite` only when the user explicitly requested replacing existing files.
5. Make no other generated-template edits unless requested.

## Script Behavior

`scripts/scaffold.py` copies every file from `assets/`, including dotfiles, preserves binary assets, replaces placeholders in UTF-8 text files, derives a lowercase bundle slug from `projectName`, and updates copyright years to the current year.

Use the script instead of manually copying files unless you are debugging or modifying the skill itself.
