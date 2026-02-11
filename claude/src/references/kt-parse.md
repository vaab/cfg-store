# Time Parsing with kt-parse

`kt-parse` is a utility for parsing timestamps and timespans. Use it in
scripts instead of `date -d` for more flexible time parsing.

## Usage

`kt-parse [-F <format>] time <time-string>`
`kt-parse [-F <format>] timespan <time-string-start>[..<time-string-end>]`

### Output Formats (-F flag)

- `full` (default): `<unix_timestamp> <YYYY-MM-DD> <HH:MM:SS> <+TZ>`
- `ts`: Unix timestamp only (e.g., `1736160300`)
- `iso`: ISO 8601 format (e.g., `2025-01-06T11:45:00+01:00`)

Output lines: 1 for `time`, 2 for `timespan` (start and end).

## Examples

```bash
# Default full output
kt-parse time "2026-01-12 02:54"
# Output: 1768182840 2026-01-12 02:54:00 +01:00

# Timestamp only
kt-parse -F ts time "2026-01-12 02:54"
# Output: 1768182840

# ISO 8601 format
kt-parse -F iso time "2026-01-12 02:54"
# Output: 2026-01-12T02:54:00+01:00

# Timespan with timestamp output
kt-parse -F ts timespan "2026-01-12 10:00..11:00"
# Output:
# 1768209600
# 1768213200

# Natural language
kt-parse time now
kt-parse -F ts time "1 week ago"

# Explicit offset overrides TZ
kt-parse time "2026-01-12 02:54+01:00"

# Incomplete spec defaults missing left-side info to minimum
kt-parse time 2026-01-01
# Output (TZ=Europe/Paris): 1767222000 2026-01-01 00:00:00 +01:00

# Timespan with partial end time (infers year/month/day from start)
kt-parse timespan "2026-01-12 02:54..03:00"
```

## Notes

- Supports english words "now", "today", "yesterday", "1 week ago", etc.
- For fallback when `kt-parse` unavailable, use `date -d "$time" +%s`
- Project code in `~/dev/rs/kal-time`, full doc in `~/dev/rs/kal-time/README.org`
