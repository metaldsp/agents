#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
# SPDX-License-Identifier: CC0-1.0

"""Format C/C++ files after Claude/Codex write/edit operations without blocking the operation."""

import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

CPP_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"}
PATCH_FILE_RE = re.compile(r"^\*\*\* (?:Add File|Update File|Move to): (.+?)\s*$", re.M)


def extract_paths(data: dict) -> list[str]:
    tool_input = data.get("tool_input") or {}
    found = []
    # Claude: Write/Edit/MultiEdit
    for key in ("file_path", "path"):
        value = tool_input.get(key)
        if isinstance(value, str):
            found.append(value)
    # Codex: apply_patch carries the patch text in tool_input.command
    command = tool_input.get("command")
    if isinstance(command, str):
        found += PATCH_FILE_RE.findall(command)
    return found


def main() -> int:
    try:
        data = json.load(sys.stdin)
    except (json.JSONDecodeError, ValueError):
        return 0
    if not isinstance(data, dict):
        return 0

    clang_format = shutil.which("clang-format")
    if not clang_format:
        return 0

    base = Path(data.get("cwd") or Path.cwd())
    for raw in extract_paths(data):
        path = Path(raw)
        if not path.is_absolute():
            path = base / path
        if path.suffix.lower() not in CPP_SUFFIXES or not path.is_file():
            continue
        subprocess.run(
            [clang_format, "-i", "--style=file", str(path)],
            check=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
