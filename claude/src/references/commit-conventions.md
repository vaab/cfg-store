# Commit Conventions

Format: `{type}: {scope}? [module]? {description} !minor?`

Where `?` marks optional parts. Note that `[module]` uses **literal square brackets** in the output (e.g., `[bash]`, `[emacs]`). Check the repo's recent commits (`git log --oneline`) to match existing conventions.

## Primary types

- `fix:` - Bug fixes
- `chg:` - Small changes to existing features
- `new:` - New features or additions

## Optional scope (no user-facing impact)

- `dev:` - Developer-side only (refactor, comments, cosmetics)
- `doc:` - Documentation only (README.org, etc.)
- `test:` - Tests only
- `pkg:` - Project management files (CLAUDE.md, .gitignore, etc.)

## Optional suffix

- `!minor` - Trivial change of no concern to any audience (typo in
  comment, whitespace fix, reordering code blocks where order doesn't
  matter)

## Examples

```
fix: [live-profile] correct ``mkdtemp`` template
new: [bash] add ``zoxide`` integration
chg: dev: [emacs] refactor keybinding setup !minor
chg: pkg: update CLAUDE.md !minor
```

## Guidelines

- Keep the first line under 72 characters
- Use double-backticks around technical terms (paths, variables, function
  names) in both title and body
- No `Co-Authored-By` line for AI agents
- Body should explain "why" when not obvious from the title
- Keep commits focused; include rationale and edge cases in the body
- Reference issues with "Refs #123" or "Fixes #123" when applicable

## Shell escaping

When commit messages contain backticks, use a HEREDOC with single-quoted
delimiter to prevent bash command substitution:

```bash
git commit -m "$(cat <<'EOF'
chg: [module] add ``technical-term`` feature
EOF
)"
```

## Amending and rebasing

When a small change logically belongs to a recent unpushed commit, prefer
amending or interactive rebasing over creating a separate commit:

- Amend when the change belongs to the most recent commit
- Use ``git rebase -i`` when the change belongs to an earlier unpushed commit
- Only do this when commits are **not yet pushed** to remote
- Only do this when the merge/rebase is trivial or easy to resolve
- Examples of good candidates: fixing a typo in a just-added file, adding
  forgotten documentation for a new feature, small fix to code just committed
