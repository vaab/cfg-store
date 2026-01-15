# Coding Guidelines

## Fail Fast & Be Strict

- Detect and report errors as early as possible.
- Fail on unexpected content; do not silently continue.
- If exactly one result is expected, fail if zero or more than one is found.
- Document failures with: clear description, actionable fix, or list of potential causes.

## Error Handling

- Never swallow exceptions silently.
- Avoid defaults that hide missing data (e.g., `dict.get(key, [])` when key must exist).
- Use assertions to enforce invariants.

## Explicitness & Immutability

- Be explicit; avoid magic behavior.
- Use named constants, not magic numbers/strings.
- Prefer immutability.

## Boundaries & State

- Validate at system boundaries (user input, external APIs); trust internal code.
- Operations must be atomic: complete fully or not at all.
- Always clean up resources (context managers, finally, RAII).

## Code Structure

- Single responsibility: one function, one job.
- Prefer idempotent operations.

## Code Changes Checklist

For each code change or addition, consider:
- Unit tests: add or update as needed.
- Integration tests: add or update as needed.
- Documentation: update README.org if behavior or usage changes.
- Build: run release build (if applicable) so user can test immediately.
