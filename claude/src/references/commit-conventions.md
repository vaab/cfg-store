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
- Include the `$` prefix when referencing shell/bash variable names
  (e.g., ``$SERVICE_DATASTORE``, not ``SERVICE_DATASTORE``)
- No `Co-Authored-By` line for AI agents
- Body should explain "why" when not obvious from the title
- Keep commits focused; include rationale and edge cases in the body
- Reference issues with "Refs #123" or "Fixes #123" when applicable

## Preflight Checklist (BLOCKING)

Before running `git commit`, perform this backtick audit:

1. Scan commit message (title AND body) for technical terms:
   - Filenames: `README.org`, `Cargo.toml`, `package.json`
   - Paths: `src/lib.rs`, `bin/test`, `~/.config/`
   - Commands/tools: `cargo`, `git`, `docshtest`
   - Flags: `-F`, `--format`
   - Variables: `$HOME`, `$PATH`
   - Function/type names: `parse_time`, `DateTime`

2. Verify each technical term is wrapped in double-backticks

3. Do NOT proceed to commit until audit passes

**Bad → Good examples:**
- `add tests for README.org` → `add tests for ``README.org```
- `run docshtest on examples` → `run ``docshtest`` on examples`
- `fix $HOME path handling` → `fix ``$HOME`` path handling`

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
amending or rebasing over creating a separate commit:

- Amend when the change belongs to the most recent commit
- Use ``git commit --fixup`` + ``git rebase --autosquash`` when the change
  belongs to an earlier unpushed commit
- Only do this when commits are **not yet pushed** to remote

### Decision process

Before committing, evaluate each change:

1. Check ``git log --oneline -5`` and ``git status`` to see recent commits
2. For each modified file/hunk, ask: "Was this introduced in a recent
   unpushed commit?"
3. If yes, the fix belongs to that commit, not a new one

Examples of changes that belong to previous commits:
- Fixing a wrong value in a just-committed entry
- Correcting a typo in a just-added file
- Adding forgotten documentation for a new feature

### Detailed workflows

For line-level staging, fixup commits, and non-interactive rebase, see
``references/git-history-rewriting.md``.

For organizing complex uncommitted changes (multiple concerns, fixup
candidates, rebase planning), use the ``/commit-plan`` skill.
