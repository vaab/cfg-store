# Searching and Inspecting AI Sessions

Use ``ai-audit`` to find, filter, and inspect AI assistant sessions
(Claude Code and OpenCode).

## Quick Self-Identification

Run ``oc whoami`` for a one-shot summary of the current session
context. All fields are conditional — sections are omitted when the
corresponding context is unavailable (no tmux, not inside an opencode
session, ``ai-audit`` not installed, etc.).

```sh
oc whoami
```

Example output:

```
Server:      http://127.0.0.1:4096 (PID 631437)
Client PID:  892267 (376 MB)
Project:     ~/.cfg-store/live-shared/opencode
Tmux pane:   %93 (ai-tmux:opencode.2)
Session:     ses_396d671a8ffePePYs28QiNwSje
Connections: 4
```

Use this when you need to know your own session ID, tmux pane, project
directory, or client PID. For searching *other* sessions or past
conversations, use the ``ai-audit`` commands below.

## Listing Sessions

```sh
# Find sessions where a specific topic was discussed
ai-audit list-sessions --search "some keyword"

# Filter by time range (uses kal-time syntax)
ai-audit list-sessions --timespan "2026-02-11"
ai-audit list-sessions --timespan "2026-02-01..2026-02-11"
ai-audit list-sessions --timespan yesterday

# Filter by project directory (exact path match, can be relative)
ai-audit list-sessions --project .
ai-audit list-sessions -p ../fyl
ai-audit list-sessions -p /home/vaab/dev/rs/ai-audit

# Combine filters
ai-audit list-sessions --timespan "2026-02-11" --search "some keyword"
ai-audit list-sessions --search "some keyword" -t claudecode
ai-audit list-sessions -p . --timespan today

# Machine-readable output
ai-audit list-sessions --search "some keyword" --json
```

### ``--search``

- **Case-sensitive** (exact substring match).
- Searches all transcript content: user/assistant text messages,
  tool_use names and inputs, and tool_result output.
- Scans Claude Code ``.jsonl`` session files and OpenCode part files.

### ``--timespan``

- Uses ``kal-time`` for parsing (same syntax as ``kt-parse timespan``).
- Accepts explicit dates (``2026-02-11``), ranges (``2026-02-01..05``),
  and natural language (``yesterday``, ``1 week ago``).
- A session is included if its activity range (started to last updated)
  **overlaps** with the timespan — not just if it started within it.
- **Caveat**: ``today`` resolves to "now to now+1day" (chrono-english
  behavior), not midnight-to-midnight. Use an explicit date
  (e.g., ``2026-02-11``) for full-day ranges.

### ``--project``

- Exact match against the session's project directory.
- The value is interpreted as a filesystem path (absolute or relative)
  and resolved to an absolute path via ``canonicalize``.
- Examples: ``-p .`` (current dir), ``-p ../fyl``, ``-p /absolute/path``.
- The ``project_dir`` field appears in all output formats (human, JSON,
  NUL).

### Output

- Output format: timestamp, session ID, type, project directory.
- All filters compose: ``--timespan``, ``--search``, ``--project``,
  and ``-t`` can be combined.

## Session Transcript

Display the full conversation transcript of a session, including
user messages, assistant messages, tool calls, and tool results.

```sh
# With explicit session ID
ai-audit transcript <session-id>

# Auto-detect current session (when called from inside a session)
ai-audit transcript

# Show only the last N entries
ai-audit transcript -n 20

# Include thinking/reasoning blocks (hidden by default)
ai-audit transcript -v

# Machine-readable output
ai-audit transcript --json
ai-audit transcript -0
```

### Session auto-detection

When no session ID is given, ``ai-audit`` auto-detects the calling
session:

1. Checks ``OPENCODE_SESSION_ID`` / ``CLAUDE_SESSION_ID`` env vars
   (authoritative, for future use).
2. Detects the provider from ``OPENCODE=1`` env var or by walking the
   process tree to find an ``opencode`` or ``claude`` parent.
3. Lists candidate sessions matching the current working directory
   (non-child sessions only).
4. Fingerprints: reads each candidate's recent transcript for a bash
   tool call containing ``ai-audit`` to identify the exact caller.

If exactly one candidate matches, it is used. If ambiguous, the
command errors with a list of candidates and asks for ``--session``.

### Transcript entries

- **User text**: ``[user]``
- **Assistant text**: ``[assistant]``
- **Tool calls**: ``[assistant/tool_use]`` with tool name and input
- **Tool results**: ``[tool_result]``
- **Thinking**: ``[assistant/thinking]`` (hidden by default, shown
  with ``-v``)

Works with both Claude Code (``.jsonl``) and OpenCode
(``storage/message/`` + ``storage/part/``) session formats.

## Current Session Detection

Detect and print the current AI session ID. Supports three detection
strategies.

```sh
# Auto-detect (env vars, process tree, fingerprinting)
ai-audit current-session

# JSON with provider info
ai-audit current-session --json

# NUL-terminated (for piping)
ai-audit current-session -0
```

### ``--match`` (text-based identification)

Find a session by matching text in its last N messages. Useful after
a crash + tmux-resurrect to identify which session was running in a
pane from visible text.

```sh
# Search for text in last 5 messages (default)
ai-audit current-session --match "visible text from pane"

# Limit to OpenCode sessions, search last 3 messages
ai-audit current-session --match "text" -t opencode -n 3

# Restrict to a specific project directory
ai-audit current-session --match "text" -p /home/vaab/dev/rs/fyl
```

- ``-n, --last-messages N``: number of recent messages to search
  (default: 5).
- Searches the same content as ``list-sessions --search`` (text,
  tool_use, tool_result) but limited to the tail of the transcript.
- Returns error if zero or multiple sessions match; use ``-t`` or
  ``-p`` to narrow.

### ``--pid`` (process-based identification)

Find a session from a running AI process PID. Reads
``/proc/<pid>/environ``, ``/proc/<pid>/cwd``, and detects the
provider from the process name or its ancestors.

```sh
# Identify session from an opencode attach process
ai-audit current-session --pid 286943

# With provider filter
ai-audit current-session --pid 286943 -t opencode
```

Detection chain:
1. Check ``/proc/<pid>/environ`` for ``OPENCODE_SESSION_ID`` /
   ``CLAUDE_SESSION_ID``
2. Detect provider from process name (``opencode``, ``claude``) or
   ancestors
3. Read working directory from ``/proc/<pid>/cwd`` or ``--dir`` in
   cmdline
4. Gather candidate sessions → fingerprint → most-recent fallback

### Auto-detection (default)

Uses the same auto-detection logic as ``transcript`` (env vars,
process tree, transcript fingerprinting). Useful as a building block
for scripts and other tools.

## Filtering Activity by Session

Activity events are **user-side only**: user messages and permission
grants. Assistant responses are not included.

Use ``ai-audit activity get --session`` to filter activity events to
specific session(s). Combine with ``list-sessions`` to first find the
session ID, then inspect its activity.

```sh
# Get activity for a specific Claude Code session
ai-audit activity get today --session 356d5816-7f72-467a-9389-ff505a9cb035

# Filter to multiple sessions
ai-audit activity get today -s ses_abc123 -s ses_def456

# Combine with identifier filter and JSON output
ai-audit activity get today --session <ID> claudecode-msg@DEV>rs/project --json
```

- ``--session`` / ``-s`` can be repeated to match multiple sessions.
- Session IDs are UUIDs for Claude Code, ``ses_*`` for OpenCode.
- When omitted, all sessions are included (no filtering).
- The ``session_id`` field appears in JSON and NUL output formats.
- In human output, session IDs are shown in brackets (UUIDs truncated
  to 8 chars).

## When to Use

When you need to find a past session where a specific topic, file,
error message, or feature was discussed. Typical scenarios:

- "Which session did we discuss X in?"
- "Find the session where we worked on feature Y"
- Locating prior context before starting related work
- Inspecting all activity (messages + permissions) from a known session
- Reading the full transcript of a session to understand what happened
