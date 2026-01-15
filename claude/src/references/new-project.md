# New Project Guidelines

## Project Name

When starting a new project, perform a quick web search to check if the
proposed name is already used by another project (especially in the same
domain/language). Check for:

- Existing packages on registries (PyPI, npm, crates.io, etc.)
- GitHub/GitLab repositories with the same name
- Well-known tools or libraries

If the name is already taken, inform the user and suggest alternative names
that are:
- Similar in spirit to the original
- Available (verify with a quick search)
- Memorable and easy to type

## Executable Name

- Use project name or derivative if possible.
- Check the name is not already used on the system or by common packages.

## Project Creation Checklist

When creating a new project, include:

- README.org: project description, usage, installation instructions. No AUTHOR field.
- License file: MIT if not explicitly specified otherwise.
- .gitignore: appropriate for the language/framework.
- CLAUDE.md: project-specific instructions for AI agents (see `agents-md-convention.md`).
- Executable symlinks: if project produces executables, link them in `~/.local/bin/` and report to user, with an actionable example to test.
