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

## See Also

- `tmux-pane-safety.md` — rules for modifying tmux panes safely
