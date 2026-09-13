---
name: dev-loop
description: Run a spec-driven implement/test/judge loop — an implementer and a unit-test writer work in parallel from the same spec, each in an isolated git worktree with a fresh context, then a fresh-context judge checks the merged result against the spec and real build/test output, looping back to implementation on failure (up to 3 attempts) before escalating to the user. Use when the user wants a feature or fix implemented with tests and an independent correctness check, or explicitly invokes dev-loop.
disable-model-invocation: true
metadata:
  author: Pier Luigi Fiorini
  license: MIT
  version: "1.0"
---

# Dev Loop

A rigid state machine for implementing a change with tests and an independent
correctness check:

```
   SCOPE
     |
     v
 +---------------------+
 | GENERATE (parallel) |<--------------------+
 | IMPLEMENT  |  TEST  |                     |
 +---------------------+                     |
     |                                       |
     v                                       |
 INTEGRATE (merge worktrees, build, test)     |
     |                                       |
     v                                       |
   JUDGE  ------ FAIL (attempt < 3) ---------+
     |
     | PASS                  FAIL (attempt == 3)
     v                            |
    DONE                      ESCALATE
```

Two isolated fresh-context agents build IMPLEMENT and TEST from the same spec, in
parallel, each in its own git worktree, so neither can see the other's work: a
test-writer that can see the implementation tends to write tests that match its bugs.
A third fresh-context JUDGE checks the merged, built, and tested result against the
spec and routes back to GENERATE on failure — an agent grading its own work misses
what it missed the first time, so judging happens in a context that never saw the
implementation attempt being made.

Every state transition is driven by a fixed rule, not vibes: the judge's verdict is a
literal `VERDICT: PASS` / `VERDICT: FAIL` line, parsed mechanically; the retry count is
a hard cap of 3 attempts.

## Process

### 1. SCOPE (you, not a sub-agent)

- Resolve the task description: whatever the user described in conversation, or the
  skill invocation's arguments. If neither exists, ask.
- Confirm the current directory is a git repo and capture the base commit:
  `git rev-parse HEAD`.
- Check `git status`. If the working tree is dirty, warn the user: worktrees for the
  parallel agents are created from the committed base ref, so uncommitted changes will
  not be visible inside them. Ask whether to proceed anyway, or whether they'd rather
  commit/stash first.
- Read the project's `CLAUDE.md` / `AGENTS.md` for build and test commands. If neither
  documents them, ask the user.
- Write `<scratchpad>/dev-loop/spec.md` — this is the *only* thing every fresh-context
  sub-agent will see, so make it self-contained:
  - Task description and concrete acceptance criteria.
  - Relevant existing files, interfaces, and conventions to follow.
  - The build and test commands.
  - An empty `## Revision Notes` section (filled in after any failed attempt).
- Set `attempt = 1`.

### 2. GENERATE (two `Agent` calls, `general-purpose`, `isolation: "worktree"`, same message)

Launch both in the **same message** so they run in parallel, each from the base commit
captured in SCOPE, each in its own worktree:

**Implementer** — prompt inlines the full contents of `spec.md` plus: "Implement only
the production code change described in the spec. Do not create or modify test files.
Follow this repository's existing conventions — read `CLAUDE.md`/`AGENTS.md` and
nearby files for style before writing. Report which files you changed and why." If
`attempt > 1`, `spec.md`'s Revision Notes are included so this attempt knows what the
previous one got wrong.

**Test-writer** — a *separate* worktree from the same base commit (so it genuinely
cannot see the implementer's new code this run), prompt inlines the full contents of
`spec.md` plus: "Write unit tests only, covering the acceptance criteria in the spec.
Do not create or modify production/implementation code. You do not have access to this
run's implementation — write against the spec'd interface/contract, not a specific
implementation, so the tests independently verify behavior instead of mirroring
implementation bugs. Match this repository's existing test framework and file layout
(inspect the project's test directory and build configuration before
writing)."

Record both results' worktree paths and branch names.

### 3. INTEGRATE (you, not a sub-agent)

- Create a third worktree from the original base commit:
  `git worktree add <integration-path> -b dev-loop/integration-<attempt> <base>`.
- Merge both branches into it:
  `git -C <integration-path> merge --no-edit <impl-branch> <test-branch>`.
  The two agents' scopes are disjoint by instruction (implementation files vs. test
  files), so this should be a clean merge. If it conflicts, that is a scope violation —
  stop, do not auto-resolve, and report it to the user.
- Run the build and test commands from `spec.md` inside `<integration-path>`. Capture
  full output and the pass/fail result. A build failure is itself a strong signal but
  still routes through JUDGE below, so there is exactly one place that produces
  revision notes.

### 4. JUDGE (one `Agent` call, `general-purpose`, fresh context, no worktree isolation)

This agent only needs to read text you hand it — it doesn't need repo write access and
shouldn't be invited to use any. Prompt inlines:

- The full contents of `spec.md`.
- The integration diff: `git -C <integration-path> diff <base>..HEAD`.
- The captured build/test output from INTEGRATE.

Brief: "Judge whether this diff satisfies every acceptance criterion in the spec,
whether the tests actually exercise the spec'd behavior (flag anything trivial or
tautological), and whether the build/tests actually passed. If FAIL, give a numbered
list of specific, actionable revision notes for the next attempt — not vague
criticism. End your report with a line that is exactly `VERDICT: PASS` or
`VERDICT: FAIL`, with nothing after it."

Parse that exact line yourself to decide the transition below — do not infer the
verdict from the surrounding prose.

### 5. Transition

- **`VERDICT: PASS`** → **DONE**. Report the diff, the judge's notes, and the test
  results to the user. Ask whether to merge `dev-loop/integration-<attempt>` into
  their actual branch — never merge or commit it automatically. Either way, remove the
  implementer and test-writer worktrees (`git worktree remove`); remove the
  integration worktree too once the user has decided what to do with it.
- **`VERDICT: FAIL`**, `attempt < 3` → remove all three worktrees for this attempt,
  append the judge's revision notes to `spec.md` under a new `### Attempt <N>`
  heading, increment `attempt`, and go back to **GENERATE**. Each retry restarts
  implementation from the original base commit, not the failed attempt's code — the
  accumulated Revision Notes carry knowledge forward, not stale code, so mistakes
  don't compound across attempts.
- **`VERDICT: FAIL`**, `attempt == 3` → **ESCALATE**. Stop looping. Do not delete the
  last integration worktree, its branch, or the judge's report — present them to the
  user and ask how they want to proceed.

## Cleanup

`git worktree list` should be clean (no leftover `dev-loop/*` worktrees) once a run
reaches DONE or the user has resolved an ESCALATE. Always remove worktrees with
`git worktree remove`, never by deleting the directory directly.
