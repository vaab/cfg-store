# Org-mode Syntax Reference

Quick reference for org-mode formatting conventions.

## Timestamps

**Active timestamps** (appear in agenda):
```
<2025-06-06 Fri>              # date only
<2025-06-06 Fri 14:30>        # date + time
<2025-06-06 Fri 14:30 +0200>  # date + time + timezone
```

**Inactive timestamps** (don't appear in agenda):
```
[2025-06-06 Fri]              # date only
[2025-06-06 Fri 14:30]        # date + time
[2025-06-06 Fri 14:30 +0200]  # date + time + timezone
```

**Rules:**
- Always include day name (Mon, Tue, Wed, Thu, Fri, Sat, Sun)
- Always include timezone as fixed UTC offset (`+0200`, `+0100`)
- Never use named timezones (`CEST`, `CET`, `UTC`)
- Use local time with explicit timezone

## Headings

```org
* Level 1
** Level 2
*** Level 3
```

- Keep an empty line above and below headings
- TODO keywords: `TODO`, `WIP`, `DONE`, `CANCELED`, `BLOCKED`, `POSTPONED`

## Body Text

- No indentation (start at column 0)
- Wrap long lines (~80 columns)

## Inline Markup

- `=text=` - domain names, app names, brand names (e.g., `=mailcow=`)
- `~text~` - code: variables, paths, function names (e.g., `~/etc/config~`)
- `[[url][description]]` - links

## Blocks

All blocks start at column 0 (no indentation):

```org
#+begin_src sh
command here
#+end_src

#+begin_quote
Quoted text here
#+end_quote

#+begin_example
Example output here
#+end_example
```

**Exception:** When a block supports a list item, indent the whole block
(including `#+begin_...` / `#+end_...`) to keep it attached to the bullet.

## Lists

```org
- First item
- Second item
  - Nested item (2-space indent)
```

## Drawers

Drawers are indented 2 spaces under their parent heading:

```org
** Heading
  :PROPERTIES:
  :KEY: value
  :END:
```

## See Also

- For task entries with LOGBOOK/CLOCK time tracking, see `org-task-format.md`.
