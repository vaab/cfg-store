# Org-mode Task Format Reference

Format for task headings with time tracking (LOGBOOK/CLOCK entries).

## File Structure

Tasks are typically under a `* Logs` super-heading:

```org
#+TITLE: Project Admin

* Logs

** DONE first task

*** Logs

**** Did the work
  :LOGBOOK:
  CLOCK: [2025-06-06 Fri 09:00 +0200]--[2025-06-06 Fri 10:30 +0200] =>  1:30
  :END:

Body text...

** WIP second task
  ...
```

## Task Heading Structure

```org
** WIP project / task title

*** Logs

**** Description of work
  :LOGBOOK:
  CLOCK: [2025-06-06 Fri 14:00 +0200]--[2025-06-06 Fri 15:30 +0200] =>  1:30
  :END:

Body text here (no indentation).
```

## Components

### Heading Line

`** STATE project / task title`

**States:**
- `TODO` - Not started
- `WIP` - Work in progress
- `BLOCKED` - Waiting on something
- `DONE` - Completed
- `CANCELED` - Abandoned
- `POSTPONED` - Deferred

**Title format:**
- Hierarchical: `project / subproject / task description`
- Can include issue/PR numbers: `project / fix bug #214`

### CLOSED Timestamp

Only for `DONE` or `CANCELED` tasks:
```org
  CLOSED: [2025-06-06 Fri 17:30 +0200]
```

### PROPERTIES Drawer (optional)

```org
  :PROPERTIES:
  :TASK_REF: 248
  :END:
```

**TASK_REF** is a link to an external task database. Do NOT add
TASK_REF when creating new tasks unless explicitly requested.

### LOGBOOK Drawer

LOGBOOK is placed at the log entry level (`****`), not at the task heading level.

**Indentation:** Exactly 2 spaces.

```org
**** Work session
  :LOGBOOK:
  CLOCK: [2025-06-06 Fri 09:30 +0200]--[2025-06-06 Fri 12:00 +0200] =>  2:30
  CLOCK: [2025-06-06 Fri 14:00 +0200]--[2025-06-06 Fri 17:30 +0200] =>  3:30
  :END:
```

**CLOCK format:**
```
CLOCK: [START]--[END] => DURATION
```

- Multiple CLOCK entries allowed (interrupted work on same day)
- Duration format: `H:MM` (e.g., `2:30` = 2 hours 30 minutes)
- Entries ordered newest first

### Body Text

No indentation (start at column 0).

## CLOCK Entry Rules

### Timestamps

- Use **inactive timestamps** `[...]` for CLOCK entries
- Always include explicit timezone (`+0200`, `+0100`)
- Never use naive datetimes or named timezones
- Use CET timezone (`+0100` winter, `+0200` summer) regardless of
  physical location

### 15-minute Resolution

Times must be on a 15-minute grid (00, 15, 30, 45):
- Round each time to nearest 15-minute mark
- Shift by no more than 7.5 minutes
- Preserve approximate total duration

Example:
- Original: 09:23 - 11:47 (2h24m)
- Rounded: 09:30 - 11:45 (2h15m)

## Task Body Structure

### Context Subheading

First subheading explains why the work was triggered:

```org
*** Context

Why this work was triggered (request, ticket, issue report).
```

The context section contains:
- What triggered the work
- Why the activity was needed
- Initial problem description

### Logs Subheading

Contains log entries for each intervention:

```org
*** Logs

**** Investigation du problème
  :LOGBOOK:
  CLOCK: [2025-06-06 Fri 09:30 +0200]--[2025-06-06 Fri 12:00 +0200] =>  2:30
  :END:

Vérifié les logs, trouvé que...

**** Correction appliquée
  :LOGBOOK:
  CLOCK: [2025-06-07 Sat 14:00 +0200]--[2025-06-07 Sat 17:30 +0200] =>  3:30
  :END:

Modifié le fichier X pour...
```

Each log entry:
- Has a descriptive title (no date - CLOCK entries provide timing)
- Has its own LOGBOOK with CLOCK entries
- Contains intervention details or reflections
- Ordered chronologically (based on CLOCK timestamps)

### Writing Style

Audience is engineers. They should be able to:
- Understand the issue
- Understand how it was dealt with
- Reproduce the fix or understand the explanation

Write with:
- Technical precision
- Clear cause → action → result flow
- Reproducible steps when applicable
- Code blocks for commands, configs, errors
- `=inline code=` for paths, variables, commands

## Complete Example

```org
** DONE project / fix performance issue
  CLOSED: [2025-06-07 Sat 17:30 +0200]

*** Context

Server was slow, users complained about response times.

*** Logs

**** Initial investigation
  :LOGBOOK:
  CLOCK: [2025-06-06 Fri 09:30 +0200]--[2025-06-06 Fri 12:00 +0200] =>  2:30
  :END:

Checked logs, found memory issues.

#+begin_src sh
grep -i "out of memory" /var/log/syslog
#+end_src

**** Applied fix
  :LOGBOOK:
  CLOCK: [2025-06-07 Sat 14:00 +0200]--[2025-06-07 Sat 17:30 +0200] =>  3:30
  :END:

Increased memory limit in config, restarted services. Problem resolved.
```
