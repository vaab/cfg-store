# Git Push Convention

## NEVER push without explicit request

**NEVER push to any remote unless the user EXPLICITLY asks you to push.**

This is a hard rule with NO exceptions. Even if:
- You just committed changes
- The workflow seems to imply pushing
- It would be "convenient"

**DO NOT PUSH.** Wait for the user to say "push", "push it", "push to remote", etc.

## When to push

Only push when the user explicitly requests it with words like:
- "push the commits"
- "push to origin"
- "push it"
- "git push"

## What to show before pushing

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
