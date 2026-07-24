#!/usr/bin/env bash
# Format C/C++ files after Claude write/edit operations without blocking the operation.

set -euo pipefail

input=$(cat)
file_path=$(printf '%s' "$input" | python3 -c "
import json, sys
data = json.load(sys.stdin)
tool_input = data.get('tool_input', {})
print(tool_input.get('file_path', tool_input.get('path', '')))
" 2>/dev/null || true)

if [[ -z "$file_path" ]]; then
    exit 0
fi

if [[ "$file_path" != /* ]]; then
    file_path="$PWD/$file_path"
fi

if [[ ! -f "$file_path" ]]; then
    exit 0
fi

case "$file_path" in
    *.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp|*.hxx) ;;
    *) exit 0 ;;
esac

clang-format -i --style=file "$file_path"
