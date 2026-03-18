# Tmux Pane Safety

Rules for interacting with tmux panes, windows, and sessions.

## Core Principle

The user's tmux layout is their workspace. Treat it like their open
editor tabs — never close, move, or resize without verification.

## Killing a Pane

**NEVER kill a pane without verifying its identity first.**

Before every `tmux kill-pane`:

1. Capture its content: `tmux capture-pane -t <target> -p | tail -5`
2. Check its command: `tmux display -t <target> -p '#{pane_current_command}'`
3. Confirm it matches what you expect (e.g., an empty shell you just created)
4. Only then kill it

If there is any doubt, **ask the user** instead of killing.

## Internal Testing (not user-visible)

When running tests or experiments for your own verification:

1. Create a **dedicated test session**: `tmux new-session -d -s test-<purpose>`
2. Do all work inside that session
3. Destroy the session when done: `tmux kill-session -t test-<purpose>`
4. Never use the user's active session for internal tests

## Splitting a Pane

Splitting is a destructive operation — it halves the target pane's
space.  Apply the same care as killing.

### Before splitting — ownership check

1. Identify your pane via `$OPENCODE_REQUEST_CONTEXT` (see
   `tmux-layout-awareness.md` § "Agent Identity in Tmux")
2. Only split **your own pane** (the OpenCode client pane or a pane
   you previously created)
3. **NEVER** split a pane belonging to the user without asking first

### Before splitting — dimensions check

Check the target pane's dimensions:

```bash
tmux display -t <target> -p '#{pane_width}x#{pane_height}'
```

**Minimum thresholds** (each resulting pane, after split):
- Width: ≥ 80 columns
- Height: ≥ 30 rows

This means:
- Do not split horizontally (`split-window -h`) if the pane is < 160 columns wide
- Do not split vertically (`split-window -v`) if the pane is < 60 rows tall

**Decision procedure** — try in order, pick the first that fits:
1. Horizontal split (`-h`, side-by-side) if pane width ≥ 160
2. Vertical split (`-v`, top/bottom) if pane height ≥ 60
3. Neither fits → ask the user (see below)

### If there is not enough space

Use the multi-choice question tool to ask the user:

```
I need a pane for the demo but the current layout is too tight.
Options:
- "Use a separate test session" — I'll create a temporary tmux session
- "I'll make room" — resize or close something, then tell me when ready
- "Skip the demo" — I'll just describe the result instead
```

### During the demo

1. Create exactly one pane with `split-window`
2. Note the new pane's id/index immediately
3. Work only in that pane
4. When done, verify the pane is yours (check content) before killing it

### Cleanup

Before removing the demo pane:

1. Verify its content matches your demo output
2. Kill only that specific pane
3. If unsure which pane is yours, list all panes and ask the user
