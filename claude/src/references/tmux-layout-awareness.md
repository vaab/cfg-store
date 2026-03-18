# Tmux Layout Awareness

How to understand which tmux window the user is looking at and
inspect its pane layout.

## The Problem

When running through OpenCode's client-server architecture, the LLM's
bash tools execute in the **server's** tmux pane — not the user's.
`$TMUX_PANE` points to the wrong pane.

## Request Context

OpenCode propagates `$OPENCODE_REQUEST_CONTEXT` into the bash
environment. This variable carries semicolon-separated metadata
fields, each formatted as `key:value`.

Example: `tmux:%127;source:tui`

The `tmux` field contains the pane ID of the client that sent the
request — i.e. the pane the user is actually looking at.

## ai-tmux

`ai-tmux` is a CLI tool that draws ASCII maps of tmux pane layouts.
It automatically reads `$OPENCODE_REQUEST_CONTEXT` to resolve the
user's window when no explicit target is given.

### Usage

```bash
# Auto-detect from $OPENCODE_REQUEST_CONTEXT (default when running via OpenCode)
ai-tmux layout

# Compact view — minimal size, preserves spatial relationships only
ai-tmux layout -c

# Explicit target (overrides auto-detect)
ai-tmux layout -t mysession:2
```

### When to Use

- Before any tmux pane manipulation, run `ai-tmux layout -c` to see
  what the user sees.
- Use the pane IDs from the output to target specific panes.
- The active pane (where the user's cursor is) is prefixed with `*`.

## Agent Identity in Tmux

From the user's perspective, "you" (the AI) **are** the OpenCode
client pane — the one they read and talk to.  This pane is identified
by `$OPENCODE_REQUEST_CONTEXT` (the `tmux` field).

`$TMUX_PANE` is the server's execution pane — it runs bash commands
behind the scenes but is typically **invisible** in the user's layout.
It is NOT the AI's identity.

**When the user says:**

| Phrase | Meaning |
|--------|---------|
| "your pane", "where you are" | The OpenCode client pane |
| "below you", "next to you" | Relative to the client pane |
| "create a pane for yourself" | Split the **client pane** to make a new adjacent pane |

**Resolution procedure:**

1. Read `$OPENCODE_REQUEST_CONTEXT` → extract the `tmux` field → that
   is your pane ID (e.g. `%1`)
2. Run `ai-tmux layout -c` to see where that pane sits spatially
3. Resolve any spatial reference ("below", "next to") relative to that
   pane

**Never** conflate the server pane (`$TMUX_PANE`) with the agent's
identity.  The server pane is an implementation detail; the client
pane is who you are in the user's workspace.

## Resolving Spatial References

When the user refers to a pane by position — "the pane on the left",
"the one above", "top right", "just below" — you MUST run
`ai-tmux layout -c` first to map the spatial description to an actual
pane ID.

**Never guess a pane ID from memory or context.** The layout may have
changed since you last looked.

Example workflow:

1. User says: "send that command to the pane on the right"
2. Run `ai-tmux layout -c` → see the layout
3. Identify which pane is to the right of the active pane
4. Use that pane's ID for the `tmux send-keys` command

## Relationship to tmux-pane-safety.md

This instruction is **independent** of `tmux-pane-safety.md`.

- **tmux-pane-safety** governs _destructive actions_ (kill, resize, split) —
  it is a safety gate.
- **tmux-layout-awareness** governs _correctness_ (targeting the right pane) —
  it is a resolution step.

Even if the user waives safety checks (e.g. "without validating"),
layout resolution via `ai-tmux layout -c` is still mandatory when
spatial references are used. Layout resolution is not "validation" — it
is the mechanism by which you determine **what** to act on.

## See Also

- `tmux-pane-safety.md` — rules for modifying tmux panes safely
