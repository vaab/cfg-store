# CLI Executable Guidelines

When creating a command-line executable:

## Interface

- Include `-h`/`--help` with clear usage and examples.
- Include `--version` flag.
- Use established argument parsing libraries (argparse, clap, cobra, etc.).
- Support `--` to separate options from positional arguments.

## Output

- Results to stdout, errors and warnings to stderr.
- Support stdin/stdout for piping when appropriate.
- Respect `NO_COLOR` env var; auto-detect TTY for colored output.
- Progress indicators for long operations (TTY only).

## Output Formats

Default output should balance human readability and parseability.

Implement in order of priority:

1. **Human-readable (mandatory)**: Default. Clear, concise. May use summaries.
2. **NUL-separated `-0` (recommended)**: Fixed-field records for GNU tools piping (`xargs -0`, `while read -d ''`). May include more data than human output.
3. **NDJSON `--json`/`-j` (optional)**: One JSON object per line. Field order: timestamp (if any) → fixed-width common → other common → optional/record-specific.

### Timestamps

- Human-readable: may omit tz offset (implies local), may omit date if user-constrained.
- NUL-separated/NDJSON: UTC float seconds since epoch.

### Data Completeness

- Human-readable: may summarize.
- NUL-separated/NDJSON: complete info if record <100KB. If larger, require explicit flag.

### Documentation

- Human-readable: include example output in README.org.
- NUL-separated/NDJSON: document available fields in README.org.

## Exit Codes

- Exit 0 on success, non-zero on error.
- Use distinct codes for different error types when useful.

## Behavior

- Fail fast with clear, actionable error messages.
- Handle SIGINT/SIGTERM gracefully (cleanup, no stack traces).
- Prefer explicit flags over implicit behavior.
- Consider `-q`/`--quiet` and `-v`/`--verbose` for output control.

## Configuration

- Use standard locations (`~/.config/<appname>/`).
- Support env vars for common options (document in `--help`).
