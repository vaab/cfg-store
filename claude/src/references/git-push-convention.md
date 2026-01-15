# Git Push Convention

## When to ask for confirmation

Ask for confirmation before pushing UNLESS the user explicitly requested
the push (e.g., "push the commits", "push to origin").

When the user explicitly asks to push, proceed directly.

When pushing as part of a larger workflow (e.g., after committing), ask first.

## What to show

Whether asking or proceeding, always summarize:
- Target remote and branch (e.g., `origin/main`)
- Number of commits to be pushed
- Brief description of changes

Example:
> "Pushing 3 commits to origin/main:
> - Add currency-tx-plot tool
> - Update README
> - Fix typo"

## Rationale

Code written or modified with AI assistance should be reviewed before being
shared with others. However, when the user explicitly requests a push, they
have already made the decision to share.
