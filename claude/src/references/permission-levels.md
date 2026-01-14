# Permission Levels

When a permission is requested or granted for a project, consider whether it
should be added at a higher level for reuse.

## Hierarchy (most specific to most general)

| Level | Location | Scope |
|-------|----------|-------|
| Project | `<project>/.claude/settings.local.json` | Single project only |
| Language/Domain | `~/dev/rs/.claude/settings.local.json` | All Rust projects |
| Global | `~/.claude/settings.json` | All projects everywhere |

## Decision Guide

**Add at Project level** when:
- Permission is project-specific (custom scripts, project binaries)
- Unusual tool only relevant to this codebase

**Add at Language/Domain level** when:
- Permission applies to all projects of that type
- Language toolchain commands (`cargo`, `npm`, `pip`, etc.)
- Domain-specific tools used across multiple related projects

**Add at Global level** when:
- Permission is universally useful (common CLI tools, documentation sites)
- You find yourself adding the same permission to multiple domain configs

## Validation

Before suggesting a permission, verify it makes sense:
- Check it's not a comment or string fragment from a multi-line command
- Use judgement: would this permission actually be useful in the settings file?
- Only suggest permissions for real, recognizable commands

## Prompt

When granting a permission, ask yourself:
> "Will I need this permission in other projects of the same type? In all projects?"

If yes, suggest adding it at the appropriate higher level.
