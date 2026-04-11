# Uncommitted Changes Triage

When a project has uncommitted changes and you need to start new work
on it, do not simply report "there are uncommitted changes, what should
I do?". Triage them so the user can make an informed decision.

## Core Principle: Delegate the Triage

**Triage MUST happen in a sub-agent, never in the active session.**
Uncommitted changes usually come from past sessions or manual edits,
not from the current task. Investigating them in the active session
pollutes its context with irrelevant diffs, transcripts, and history.

The active session's job is ONLY:

1. Detect the dirty tree (via `pre-work-checklist.md`)
2. **Delegate** triage to a sub-agent using the prompt below
3. Receive the structured report
4. Present options to the user
5. Execute the user's choice (possibly via another delegation)

## Hard Blocks

- NEVER `git stash`, `git checkout --`, `git restore`, `git reset --hard`,
  `git clean` — see `references/undo-safety.md`
- NEVER commit on the user's behalf without explicit approval
- NEVER proceed with the original task until uncommitted changes are
  resolved (or the user explicitly says "leave them and proceed")
- NEVER let the triage pollute the active session's context — delegate

## Sub-Agent Delegation

Use `task()` with the `deep` category. Read-only investigation, no
skills required.

```typescript
task(
  (category = "deep"),
  (load_skills = []),
  (description = "Triage uncommitted changes"),
  (run_in_background = false),
  (prompt = "<see template below, filled in>"),
);
```

### Sub-Agent Prompt Template

Copy verbatim, filling `<...>`:

    TASK: Investigate the uncommitted changes in <absolute project dir>
    and produce a triage report. DO NOT modify any file. DO NOT run
    mutating git commands. READ-ONLY investigation.

    EXPECTED OUTCOME: A structured YAML report (schema below) that
    lets the user decide what to do with each group of uncommitted
    changes. For every group you MUST provide: origin (AI session vs
    user edit), session ID and date if applicable, intent, completion
    state, and a suggested action with a ready-to-use commit message.

    REQUIRED TOOLS: Bash (git, ai-audit, stat, ls), Read, Grep.

    MUST DO:

    1. Enumerate uncommitted changes:
       - `git -C <dir> status --porcelain=v2 -uall`
       - `git -C <dir> diff --stat` and `git -C <dir> diff --staged --stat`
       - `stat --format='%y  %n' <file>` for each modified/untracked
         file to capture mtimes.
       - `git -C <dir> log --oneline -20` to see recent committed work.

    2. Group the changed files into logical "concerns":
       - Prefer path topology (same directory / same module).
       - Cluster by mtime (files modified within minutes of each other
         are likely the same session).
       - A group may contain a single file.

    3. For each group, find the likely origin by cross-referencing
       with `ai-audit` (see `~/.claude/references/session-search.md`):

       a. `ai-audit list-sessions -p <dir> --timespan <earliest_mtime>..<latest_mtime>`
          to list sessions active around the group's mtime window.
       b. `ai-audit list-sessions -p <dir> --search "<file basename>"`
          and `--search "<distinctive symbol from the diff>"` to catch
          sessions that touched the file but ran outside the mtime
          window (e.g., long-running sessions).
       c. For each candidate session:
          `ai-audit transcript <session-id> -n 30` — look for:
             - the user message that triggered the change (QUOTE IT),
             - assistant tool_use entries editing the file,
             - the final assistant message (was it "done", a question,
               an error, or did the session just end mid-work?),
             - signs of an abandoned session (no final status, crash,
               user left).
       d. If no session touched the file in the relevant window,
          classify as "user_manual" and infer intent from the diff.
       e. If evidence is mixed or ambiguous, classify as "unknown"
          and report WHAT you tried and WHY you could not decide.

    4. Synthesize per group:
       - Origin type + session id + session date window
       - Intent: one paragraph, quoting the user's own words when
         possible
       - Completion state: finished | abandoned_mid_work | unknown
       - Risk: anything obvious from the diff (broken build, TODOs
         left, type errors, half-finished refactor)
       - Suggested action with a ready commit message following
         `~/.claude/references/commit-conventions.md`

    MUST NOT DO:

    - Do NOT modify any file.
    - Do NOT run git stash, commit, reset, restore, checkout, clean,
      rebase, or any other mutating git command.
    - Do NOT install tools.
    - Do NOT start the user's next task — only triage what exists.
    - Do NOT guess aggressively. If origin is unclear, say "unknown"
      and explain what you tried and why it was inconclusive.
    - Do NOT dump full diffs or full transcripts in the report —
      summarise.

    CONTEXT:

    - Project directory: <absolute path>
    - Active session's next task (for awareness only, do NOT
      execute): <one-line summary>
    - Tools reference: ~/.claude/references/session-search.md
    - Commit format: ~/.claude/references/commit-conventions.md

    OUTPUT FORMAT (YAML, nothing else before or after):

    groups:
      - id: G1
        files:
          - path: path/to/file
            state: MODIFIED|STAGED|UNTRACKED|DELETED|RENAMED
            mtime: "2026-04-10 14:32"
        origin:
          type: ai_session|user_manual|mixed|unknown
          session_id: ses_xxx | null
          session_date: "2026-04-10 14:00..14:40" | null
          evidence: "what made you decide this origin"
        intent: >
          One paragraph explaining WHY the changes were made. Quote
          the user's own words verbatim when possible.
        completion: finished|abandoned_mid_work|unknown
        last_activity: "2026-04-10 14:40"
        risk: "none" | "<description>"
        suggested_action:
          primary: commit|discard|keep_pending
          commit_message: "chg: [module] description"   # if primary=commit
          rationale: "why this is the best action"

    summary:
      total_groups: N
      commit_candidates: M
      discard_candidates: K
      unknown: U
      blockers:
        - "anything preventing safe action, if any"

## Presenting the Triage to the User

After receiving the report, present it compactly:

    Found uncommitted changes in <project>. Triage:

    1. <G1 short description>  (<N files>)
       Origin   : AI session ses_xxx (2026-04-10 14:00..14:40)
                  or  user manual edit
       Intent   : <one-line summary>
       State    : finished / abandoned / unknown
       Risk     : <none | description>
       Suggested: commit as `chg: [module] ...`

    2. <G2 short description>
       ...

    Options:
      [A] Commit all groups with the suggested messages (one commit per group)
      [B] Commit all as a single commit
      [C] Show details of group N (reply: "detail N")
      [D] Discard group(s) — reply with explicit group IDs
      [E] Leave as-is and proceed with the new task

    What do you want to do?

Always include session IDs and dates verbatim so the user can jump
back to the original session if they need to.

## Executing the User's Choice

- **A or B (commit)**: delegate to the `commit-plan` skill, passing
  the triage report as context so commit messages are meaningful and
  grouped correctly. Do NOT craft commits in the active session.
- **C (detail)**: re-invoke the sub-agent with
  `session_id=<previous>` and ask for the expanded detail on the
  requested group. Never start fresh.
- **D (discard)**: require explicit per-group confirmation with file
  paths echoed back. Follow `references/undo-safety.md` — prefer
  surgical `git checkout -- <file>` on a per-file basis over bulk
  resets.
- **E (leave as-is)**: record the triage summary at the top of the
  active session's context so the new work is aware of the pending
  changes, and proceed.

## When Triage Is Not Needed

Skip triage when:

- Working tree is clean.
- The uncommitted changes are tiny AND obviously from the current
  active session (edits you just made minutes ago in this very turn).
- The user has already said "ignore the uncommitted changes, just
  proceed" for this task.

## Why This Workflow Exists

Without triage, uncommitted changes surface as a vague "there is
stuff here, what do I do?" question. The user then has to remember
which session left it, what it was about, and whether it's safe to
commit. That context usually lives in a past session transcript —
exactly what `ai-audit` can retrieve. Delegating the lookup to a
sub-agent keeps this archaeology out of the active session, which is
working on something else.

## See Also

- `references/pre-work-checklist.md` — safety rules for dirty trees
- `references/session-search.md` — `ai-audit` commands
- `references/commit-conventions.md` — commit message format
- `references/undo-safety.md` — safe discarding of changes
- `references/global-rules-management.md` — how to commit rule changes
