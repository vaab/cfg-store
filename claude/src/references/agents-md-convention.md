# AGENTS.md Convention

When creating any `CLAUDE.md` or `AGENTS.md` file (containing instructions,
documentation, or context for AI agents), follow this convention. This applies
to project roots, subdirectories, or any location within a repository.

## Structure

1. **Create `AGENTS.md`** as the actual file containing the documentation
2. **Create `CLAUDE.md`** as a symlink pointing to `AGENTS.md`

```bash
ln -s AGENTS.md CLAUDE.md
```

## Rationale

- `AGENTS.md` is tool-agnostic and works with any AI coding assistant
- The symlink ensures Claude Code still finds and reads the file
- Other tools (Cursor, Copilot, etc.) can also use `AGENTS.md`

## When encountering existing incorrect structure

If you find a project with a regular `CLAUDE.md` file that should follow this
convention (i.e., it contains project documentation, not Claude-specific config):

1. Rename `CLAUDE.md` to `AGENTS.md`
2. Create symlink: `ln -s AGENTS.md CLAUDE.md`
3. Commit the change with message: `chg: pkg: rename CLAUDE.md to AGENTS.md with symlink`

## Committing

`AGENTS.md` and its `CLAUDE.md` symlink are a logical pair - always commit
them together:

- When creating: add both files in the same commit
- When modifying `AGENTS.md`: check if `CLAUDE.md` symlink exists but is
  untracked, and include it in the same commit
- When renaming: include both the rename and symlink creation in one commit

## Exceptions

This convention applies to project documentation files. It does NOT apply to:
- `~/.claude/CLAUDE.md` (global user config)
- `.claude/` directory contents (Claude Code internal config)
