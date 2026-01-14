# AGENTS.md Convention

When asked to create a `CLAUDE.md` file for a project (containing project-specific
instructions, documentation, or context for AI agents), follow this convention:

## Structure

1. **Create `AGENTS.md`** as the actual file containing the documentation
2. **Create `CLAUDE.md`** as a symlink pointing to `AGENTS.md`

```bash
# In project root:
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

## Exceptions

This convention applies to project documentation files. It does NOT apply to:
- `~/.claude/CLAUDE.md` (global user config)
- `.claude/` directory contents (Claude Code internal config)
