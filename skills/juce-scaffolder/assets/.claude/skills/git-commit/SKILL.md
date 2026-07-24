---
name: git-commit
description: Use when creating Git commits to follow the Conventional Commits specification. Delegate to the git-commit agent so it can inspect, stage, and commit the intended changes.
allowed-tools: Agent
---

# Conventional Commits

Delegate commit creation to the `git-commit` agent. Forward user-specified scope, wording, and
file constraints; the agent inspects the status and diff, stages files if necessary, and creates
the commit.
