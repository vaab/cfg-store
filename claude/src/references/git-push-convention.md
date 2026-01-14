# Git Push Convention

## Always ask confirmation before pushing

NEVER push to a remote repository without explicit user confirmation, even if
the user previously asked to "push" in the same conversation.

Before pushing, always ask something like:
> "Ready to push to origin/master. Proceed?"

## Rationale

Code written or modified with AI assistance should be reviewed before being
shared with others. Pushing is an irreversible action that affects collaborators
and CI/CD pipelines.

## What to show before asking

When asking for push confirmation, summarize:
- Target remote and branch (e.g., `origin/master`)
- Number of commits to be pushed
- Brief description of changes

Example:
> "Ready to push 3 commits to origin/master:
> - Add currency-tx-plot tool
> - Update README
> - Fix typo
>
> Proceed?"

## Exceptions

None. Always ask, even for trivial changes.
