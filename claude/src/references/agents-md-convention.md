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

## Local instruction files (`.local.md`)

`CLAUDE.local.md` is a non-versioned, per-machine override file (typically
gitignored).  Unlike `AGENTS.md`/`CLAUDE.md`, **there is no `AGENTS.local.md`
convention** — no tool reads it natively.

- **Claude Code**: reads `CLAUDE.local.md` automatically.
- **OpenCode**: does **not** read `.local.md` files automatically.  You must
  add it to the `instructions` array in `opencode.json`:

```json
{
  "instructions": ["CLAUDE.local.md"]
}
```

When creating a `CLAUDE.local.md` in a project, always check that
`opencode.json` includes the entry above so both tools read it.

The AGENTS.md/CLAUDE.md symlink convention does **not** apply to `.local.md`
files — just create `CLAUDE.local.md` directly as a regular file.

## Exceptions

This convention applies to project documentation files. It does NOT apply to:
- `~/.claude/CLAUDE.md` (global user config)
- `.claude/` directory contents (Claude Code internal config)
- `CLAUDE.local.md` (local override — see above)
