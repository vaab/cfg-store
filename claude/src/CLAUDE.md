# Global Claude Code Instructions

## User Identity

- Name: Valentin Lab
- Email: valentin.lab@kalysto.org

## References

References below are relative to this file.

- For parsing time string using command line, use `kt-parse` instead of `date -d`. READ `references/kt-parse.md`.
- For new projects guidelines, READ `references/new-project.md`.
- **BLOCKING**: Before writing ANY commit message, READ `references/commit-conventions.md`. No exceptions.
- **BLOCKING**: Before ANY `git push` command, you MUST first READ `references/git-push-convention.md` and follow its rules. No exceptions.
- When granting permissions, consider the appropriate level (project/domain/global). READ `references/permission-levels.md`.
- Before creating or writing any `CLAUDE.md` or `AGENTS.md` file, READ `references/agents-md-convention.md`.
- When writing code, READ `references/coding-guidelines.md`.
- When creating a CLI executable, READ `references/cli-guidelines.md`.
- When writing org-mode content, READ `references/org-syntax.md`.
- Before creating or modifying this file or any file in `references/`, READ `references/global-rules-management.md`.
- For debugging Claude Code permission issues, use `ai-audit permissions <session-id>`.
- **BLOCKING**: Before ANY release-related action (versioning, naming, publishing), you MUST first READ `references/release-guidelines.md` and get explicit user approval for key decisions. No exceptions.
- When troubleshooting Claude Code crashes or issues, READ `references/claude-troubleshooting.md`.
- **BLOCKING**: Before modifying `~/.config/opencode/opencode.json`, READ `references/opencode-config.md` and follow the backup/validation procedure. No exceptions.
- **BLOCKING**: When a mistake is identified (by user or self), IMMEDIATELY invoke the `/learn-from-mistake` skill. Do NOT just acknowledge - run the full workflow. No exceptions.
