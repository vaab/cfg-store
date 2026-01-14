# Time Parsing with kt-parse

`kt-parse` is a utility for parsing timestamps and timespans. Use it in
scripts instead of `date -d` for more flexible time parsing.

## Usage

`kt-parse time <time-string>`
`kt-parse timespan <time-string-start>[..<time-string-end>]`

Output line:

`<unix_timestamp> <YYYY-MM-DD> <HH:MM:SS> <+TZ>`

- 1 line for `time`,
- 2 lines for start and end in case of `timespan` requested.

Examples:

```bash
# Parse english words to timestamp and formatted full date time and tz
kt-parse time now
kt-parse time "1 week ago"

kt-parse time "2026-01-12 02:54"         ## no offset specified => TZ env var used
kt-parse time "2026-01-12 02:54+01:00"   ## offset specified so it is used
# Output (TZ=Europe/Paris): 1768182840 2026-01-12 02:54:00 +01:00

# Parse incomplete spec to timestamp and formatted full date time and tz
kt-parse time 2026-01-01                 ## defaults left side missing info to minimum
# Output (TZ=Europe/Paris): 1767222000 2026-01-01 00:00:00 +01:00

# Parse a timespan (returns start and end timestamps relative to now)
kt-parse timespan "2026-01-12 02:54..2026-01-13 03:00"  # partial (no seconds)
kt-parse timespan "2026-01-12..2026-01-15"              # partial (no time)
kt-parse timespan "2026-01-12 02:54..03:00"             # partial (year/month/day infered)
kt-parse timespan "2026-01-12 02:54..03:00"             # partial (year/month/day infered)
# Output: two lines with start/end timestamps

# Extract just the unix timestamp
kt-parse time "2026-01-12 02:54" | awk '{print $1}'
```

## Notes

- Support english words "now", "today", ...
- For fallback when `kt-parse` unavailable, use `date -d "$time" +%s`
- Project code in `~/dev/rs/kal-time`, full doc in `~/dev/rs/kal-time/README.org`
