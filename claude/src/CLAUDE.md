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
- **BLOCKING**: Before creating ANY CLI executable, READ `references/cli-guidelines.md`. No exceptions.
- **BLOCKING**: Before writing ANY org-mode content, READ `references/org-syntax.md`. No exceptions.
- **BLOCKING**: Before creating or modifying this file or ANY file in `references/`, READ `references/global-rules-management.md`. No exceptions.
- For debugging Claude Code permission issues, use `ai-audit permissions <session-id>`.
- **BLOCKING**: Before ANY release-related action (versioning, naming, publishing), READ `references/release-guidelines.md` and get explicit user approval for key decisions. No exceptions.
- When troubleshooting Claude Code crashes or issues, READ `references/claude-troubleshooting.md`.
- **BLOCKING**: Before modifying `~/.config/opencode/opencode.json`, READ `references/opencode-config.md` and follow the backup/validation procedure. No exceptions.
- **BLOCKING**: When a mistake is identified (by user or self), IMMEDIATELY invoke the `/learn-from-mistake` skill. Do NOT just acknowledge - run the full workflow. No exceptions.
