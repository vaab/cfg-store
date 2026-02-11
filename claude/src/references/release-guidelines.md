# Release Guidelines

## Decisions Requiring Explicit Approval

Before proceeding, ALWAYS ask the user for these decisions:
- Version numbers for releases
- Package/crate names
- License choices
- Publishing to registries (crates.io, npm, PyPI, etc.)
- Any irreversible action

**Never assume defaults for these.** Even if a choice seems obvious, ask first.

---

**Always use the `/release` skill** when preparing a package for release.
Do not attempt manual releases or skip verification steps.

The skill provides:
- Subagent-based setup verification (keeps main context clean)
- Git state and version alignment checks
- Build testing before publishing
- Post-release automation review

## Invoking the Skill

Simply run `/release` and follow the workflow. The skill handles all verification
and guides through each step.

## Setup Documentation

For detailed setup instructions, see the `/release` skill's references:
- `~/dev/claude/release/references/pkg-release-setup.md` - Main setup guide
- `~/dev/claude/release/references/pkg-release-setup-rust.md` - Rust/Cargo
- `~/dev/claude/release/references/pkg-release-setup-nodejs.md` - Node.js/npm
- `~/dev/claude/release/references/pkg-release-setup-python.md` - Python/Hatch

## See Also

- `commit-conventions.md` - Commit message format
