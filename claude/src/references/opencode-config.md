# OpenCode Configuration

Location: `~/.config/opencode/opencode.json`

## Editing Procedure (MANDATORY)

Before modifying this file, follow these steps:

### 1. Create Backup

```bash
cp ~/.config/opencode/opencode.json ~/.config/opencode/opencode.json.bak
```

### 2. Make Changes

Edit the file as needed.

### 3. Validate JSON

```bash
jq . ~/.config/opencode/opencode.json > /dev/null
```

If this fails, restore from backup:
```bash
cp ~/.config/opencode/opencode.json.bak ~/.config/opencode/opencode.json
```

### 4. Verify OpenCode Still Works

```bash
opencode --version
```

If opencode fails to start, restore from backup.

## Permission Structure

```json
{
  "permission": {
    "read": {
      "*": "allow|deny|ask",
      "pattern": "allow|deny|ask"
    },
    "edit": {
      "*": "allow|deny|ask",
      "/path/**": "allow|deny|ask"
    },
    "bash": {
      "*": "ask",
      "command *": "allow",
      "exact-command": "allow"
    },
    "webfetch": "allow|deny|ask",
    "websearch": "allow|deny|ask"
  }
}
```

## Bash Permission Patterns

- `"command *"` - command with any arguments
- `"command"` - exact command (no arguments)
- `"*"` - fallback for unmatched commands

## Values

- `"allow"` - auto-approve without prompting
- `"deny"` - auto-reject
- `"ask"` - prompt each time
