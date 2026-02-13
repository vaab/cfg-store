# Global Claude Code Instructions

## User Identity

- Name: Valentin Lab
- Email: valentin.lab@kalysto.org

## References

References below are relative to this file.

- **BLOCKING**: Before parsing time strings via command line, use `kt-parse` instead of `date -d`. READ `references/kt-parse.md`. No exceptions.
- **BLOCKING**: Before creating ANY new project, READ `references/new-project.md`. No exceptions.
- **BLOCKING**: Before writing ANY commit message, READ `references/commit-conventions.md`. No exceptions.
- **BLOCKING**: Before ANY `git push` command, READ `references/git-push-convention.md` and follow its rules. No exceptions.
- **BLOCKING**: Before granting ANY permissions, READ `references/permission-levels.md` and consider the appropriate level (project/domain/global). No exceptions.
- **BLOCKING**: Before creating or writing ANY `CLAUDE.md` or `AGENTS.md` file, READ `references/agents-md-convention.md`. No exceptions.
- **BLOCKING**: Before writing ANY code, READ `references/coding-guidelines.md`. No exceptions.
- **BLOCKING**: Before starting ANY significant change in an existing project, READ `references/pre-work-checklist.md` and ensure a clean git save point. No exceptions.
- **BLOCKING**: After implementing ANY feature, fix, or change, you MUST verify it works BY YOURSELF. NEVER ask the user to test, check, or verify your work. READ `references/self-verification.md` and write automated tests. No exceptions.
- **BLOCKING**: Before writing or modifying ANY test, READ `references/test-engineering.md`. Tests verify the SPECIFICATION, not the implementation. Write tests first, then fix code. No exceptions.
- **BLOCKING**: Before creating ANY CLI executable, READ `references/cli-guidelines.md`. No exceptions.
- **BLOCKING**: Before structuring a Rust CLI project's `src/` directory, READ `references/rust-cli-structure.md`. No exceptions.
- **BLOCKING**: Before writing ANY org-mode content, READ `references/org-syntax.md`. No exceptions.
- **BLOCKING**: Before creating or modifying this file or ANY file in `references/`, READ `references/global-rules-management.md`. No exceptions.
- **BLOCKING**: Before running any project operation (build/test/deploy, device interaction via adb/fastboot), READ the repo's `AGENTS.md` first. Use documented environment (e.g., Java version) and prefer helper scripts over raw commands. No exceptions.
- **BLOCKING**: Before committing ANY code that interacts with external systems (hardware, APIs, filesystem, CLI), READ `references/integration-testing-protocol.md` and verify with REAL dependencies. Unit tests alone are insufficient. No exceptions.
- For debugging Claude Code permission issues, use `ai-audit permissions <session-id>`.
- **BLOCKING**: Before searching past sessions or identifying a session from a substring, READ `references/session-search.md` and use `ai-audit list-sessions --search`. No exceptions.
- **BLOCKING**: Before ANY release-related action (versioning, naming, publishing), READ `references/release-guidelines.md` and get explicit user approval for key decisions. No exceptions.
- When troubleshooting Claude Code crashes or issues, READ `references/claude-troubleshooting.md`.
- **BLOCKING**: Before modifying `~/.config/opencode/opencode.json`, READ `references/opencode-config.md` and follow the backup/validation procedure. No exceptions.
- **BLOCKING**: Before upgrading ANY globally installed software or tool, READ `references/software-upgrades.md`. No exceptions.
- Before interacting with ANY desktop GUI application (launching, automating, screenshotting), READ `references/desktop-app-orchestration.md`. Covers Signal Desktop and general xdotool/wmctrl workflow.
- **BLOCKING**: When a mistake is identified (by user or self), IMMEDIATELY invoke the `/learn-from-mistake` skill. Do NOT just acknowledge - run the full workflow. No exceptions.
  - **Hard triggers** (invoke skill immediately, NO clarifying questions first): "you made a mistake", "that's wrong", "you violated X", "you ignored instructions", "mistake again"
  - When these phrases appear, your FIRST action must be to invoke the skill - do not ask "what did I do wrong?"
