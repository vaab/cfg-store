# Claude Code Troubleshooting

Known issues and fixes for Claude Code CLI. These are bugs in Claude
Code itself, not in the underlying models. Note: OpenCode does not
exhibit these issues.

## Issue: SIGABRT crash on startup (StringImpl assertion)

### Symptoms

- Claude CLI crashes with SIGABRT immediately on launch
- `--help` and `--version` work, but any session crashes
- Crash is intermittent (sometimes works, sometimes crashes)
- Coredump shows crash at `WTF::StringImpl` assertion (bit 31 set on string length)
- Stack trace includes addresses like `0x4006f2b` → `0x471918a` in the claude binary

### Diagnosis

1. Check for crashes:
   ```sh
   coredumpctl list | grep claude
   ```

2. Check for oversized session files:
   ```sh
   find ~/.claude/projects -name "*.jsonl" -size +100M
   ```

3. Look for abnormally large session files (normal sessions are < 50MB,
   problematic ones can be 1GB+)

### Root Cause

A corrupt or oversized session `.jsonl` file in `~/.claude/projects/` triggers
a string length overflow in Bun's WebKit StringImpl code. The file may have
few messages but enormous size due to corruption or embedded binary data.

### Fix

Move or delete the oversized session file:

```sh
# Find large session files
find ~/.claude/projects -name "*.jsonl" -size +100M -ls

# Move the problematic file (preserves data for analysis)
mv ~/.claude/projects/<project>/<session-id>.jsonl ~/tmp/

# Or delete it
rm ~/.claude/projects/<project>/<session-id>.jsonl
```

The sessions-index.json will have a stale reference but Claude handles this
gracefully.

### Related

- Bun GitHub Issue #23213: "Repeated crashes using Claude Code in Bun"
- Affects Claude Code versions 2.1.9 through 2.1.12 (at least)
