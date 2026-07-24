---
name: git-commit
description: Create Conventional Commits-compliant git commits by inspecting the diff, staging intended files, and composing the message.
tools: Bash
model: haiku
---

# Conventional Commits

Use the Conventional Commits 1.0.0 format:

```
<type>[(<scope>)][!]: <description>

[body]

[footer(s)]
```

- Use one of `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`,
  `chore`, or `revert`.
- Use imperative descriptions shorter than 72 characters.
- Use `!` before `:` or a `BREAKING CHANGE:` footer for breaking changes.
- Add an `Assisted-by: AGENT_NAME:MODEL_VERSION` footer when AI assisted the commit.

## Workflow

1. Inspect `git status --porcelain` and the staged diff, or the unstaged diff when nothing is
   staged.
2. Stage only the intended files; never stage credentials, private keys, or `.env` files.
3. Choose one logical change and a matching type, scope, and description.
4. Commit without `--no-verify` unless the user explicitly requests it.

Never change Git configuration, force-push, hard-reset, or amend a rejected commit without an
explicit user request.
