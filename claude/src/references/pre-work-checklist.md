# Pre-Work Checklist for Existing Projects

Before starting any significant change in an existing project, ensure
you have a clean save point.

## Git Save Point

1. **Check for uncommitted changes**: Run `git status`
2. **If dirty working tree**: STOP. Report the uncommitted changes to
   the user and ASK what they want to do. Do NOT proceed until the
   user explicitly tells you how to handle them.
   - NEVER run `git stash` — this hides user work and risks data loss.
   - NEVER commit on the user's behalf without explicit approval.
   - NEVER discard, reset, or checkout over uncommitted changes.
   - The ONLY safe action is to REPORT and WAIT.
   - To investigate WHERE the changes came from (past sessions, user
     manual edits, their intent), delegate to a sub-agent per
     `references/uncommitted-changes-triage.md`. Do NOT investigate
     them in the active session — it pollutes context.
3. **Confirm clean state**: `git status` must show a clean working
   tree with nothing to commit

## Requirements Pin (when reuse cues are present)

If the user's request mentions reusing existing code ("same strategy
as", "like current-session does", names a specific function), write
these bullets before coding:

1. **User constraint** (quote): what the user said to reuse
2. **Code path to reuse**: exact function/module names
3. **Non-goals**: 1-2 things you must NOT do (e.g., "do NOT invent
   alternative detection strategies")

This takes 30 seconds and prevents hallucinated requirements.

## Why

A clean git save point before starting work ensures:

- Easy rollback if the change goes wrong
- Clear separation between pre-existing changes and new work
- Clean diffs that only show the new feature/fix
- Safe experimentation (you can `git checkout` back only because you committed first)
