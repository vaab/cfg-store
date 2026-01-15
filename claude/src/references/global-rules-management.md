# Global Rules Management

Guidelines for maintaining global Claude Code instructions.

## Repository Location

Global rules live in `~/.cfg-store/live-shared/claude/src/` and are
symlinked to `~/.claude/`:

```
~/.claude/CLAUDE.md -> ~/.cfg-store/live-shared/claude/src/CLAUDE.md
~/.claude/references/ -> ~/.cfg-store/live-shared/claude/src/references/
```

## After Making Changes

Always commit changes to the cfg-store repository:

```sh
cd ~/.cfg-store/live-shared/claude
git add -A
git commit -m "chg: [claude] <description>"
git push
```

## Best Practices

- Keep instructions concise and actionable
- Use reference files for detailed guidelines (progressive disclosure)
- Trigger references with clear conditions in CLAUDE.md
- Test new instructions before committing
- Document the "why" in commit messages

## Reference File Guidelines

- One topic per file (e.g., `commit-conventions.md`, `org-syntax.md`)
- Use descriptive filenames (lowercase, hyphenated)
- Start with a brief purpose statement
- Include examples where helpful
- Cross-reference related files in "See Also" sections

## Consolidation Principles

When the same instructions apply to multiple projects:

1. Move generic rules to global references
2. Keep project-specific overrides in project CLAUDE.md
3. Avoid duplication between global and project instructions
