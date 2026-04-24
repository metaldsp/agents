#!/usr/bin/env python3
"""Scaffold a JUCE plugin project from this skill's bundled template."""

from __future__ import annotations

import argparse
import datetime as _dt
import re
import shutil
import sys
from pathlib import Path


PROJECT_NAME_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_+-]*$")
COPYRIGHT_RE = re.compile(r"Copyright \(c\) \d{4} Pier Luigi Fiorini")


def bundle_slug(project_name: str) -> str:
    slug = re.sub(r"[^a-z0-9-]+", "-", project_name.lower()).strip("-")
    return slug or "plugin"


def validate_project_name(project_name: str) -> None:
    if not PROJECT_NAME_RE.match(project_name):
        raise ValueError(
            "project name must start with a letter or underscore and contain "
            "only letters, numbers, underscores, plus signs, or hyphens"
        )


def iter_files(root: Path) -> list[Path]:
    return sorted(path for path in root.rglob("*") if path.is_file())


def render_text(text: str, project_name: str, project_description: str) -> str:
    year = str(_dt.date.today().year)
    replacements = {
        "<PROJECT_NAME>": project_name,
        "<PROJECT_DESCRIPTION>": project_description,
        "<PROJECT_SLUG>": bundle_slug(project_name),
    }
    for old, new in replacements.items():
        text = text.replace(old, new)
    return COPYRIGHT_RE.sub(f"Copyright (c) {year} Pier Luigi Fiorini", text)


def copy_file(src: Path, dst: Path, project_name: str, project_description: str) -> None:
    try:
        text = src.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        shutil.copy2(src, dst)
        return

    dst.write_text(render_text(text, project_name, project_description), encoding="utf-8")
    shutil.copystat(src, dst)


def scaffold(
    target: Path,
    project_name: str,
    project_description: str,
    overwrite: bool,
) -> list[Path]:
    skill_dir = Path(__file__).resolve().parents[1]
    assets_dir = skill_dir / "assets"
    files = iter_files(assets_dir)
    destinations = [target / src.relative_to(assets_dir) for src in files]
    conflicts = [dst for dst in destinations if dst.exists()]

    if conflicts and not overwrite:
        return conflicts

    for src, dst in zip(files, destinations):
        dst.parent.mkdir(parents=True, exist_ok=True)
        copy_file(src, dst, project_name, project_description)

    return []


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--target", default=".", help="directory to scaffold into")
    parser.add_argument("--name", required=True, help="CMake project name")
    parser.add_argument("--description", required=True, help="README description")
    parser.add_argument("--overwrite", action="store_true", help="replace existing generated paths")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    target = Path(args.target).resolve()

    try:
        validate_project_name(args.name)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if not target.exists() or not target.is_dir():
        print(f"error: target directory does not exist: {target}", file=sys.stderr)
        return 2

    conflicts = scaffold(target, args.name, args.description, args.overwrite)
    if conflicts:
        print("error: generated paths already exist:", file=sys.stderr)
        for path in conflicts:
            print(f"  {path}", file=sys.stderr)
        return 1

    print(f"scaffolded {args.name} into {target}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
