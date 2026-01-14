# AGENTS.md

This file provides guidance to AI agents when working with code in this repository.

## Security - CRITICAL

**This repository is PUBLIC. Never commit sensitive information.**

Before any commit, verify that changes do NOT contain:
- Passwords, API keys, tokens, or secrets
- Private SSH keys or certificates
- Personal identifiable information (real names, addresses, phone numbers)
- Public IP addresses or network configurations exposing infrastructure
- Database credentials or connection strings
- Any file matching: `.env`, `credentials.*`, `secret.*`, `*.pem`, `*.key`
- Files matching `*/doc/admin.org` (personal notes, not for version control)

Note: Short hostnames (e.g., `wen`, `kahn`, `yin`) used for per-host configuration are acceptable.

If you encounter sensitive data in the codebase, warn the user immediately and do not commit.

## Repository Overview

This is a modular dotfiles/configuration repository managed by `0k-cfg` (from `~/dev/sh/0k-cfg`). Each directory is a self-contained **charm** (module) that manages configuration for a specific tool or service.

## Common Commands

```bash
# Deploy/apply configuration modules (requires 0k-cfg installed)
cfg setup <module-name>

# List available modules
ls -d */

# Run a module's setup hook directly (if testing)
bash <module>/hooks/setup
```

## Architecture

### Module Structure

Each module follows this pattern:
```
module-name/
├── metadata.yml      # Linking rules and dependencies
├── hooks/
│   ├── setup         # Executed during deployment
│   └── install       # Pre-requisite checks
├── src/              # Config files symlinked to $HOME
├── bin/              # Executables linked to ~/.local/bin
├── share/            # Shared libraries/data (see below)
└── relations/        # Dependencies on other modules
```

### Automatic Linking by ``cfg``

Beyond `metadata.yml` rules, `cfg` automatically handles certain directories:

- `bin/*` → `~/.local/bin/`
- `share/fonts/*` → `~/.local/share/fonts/`
- `share/applications/*` → `~/.local/share/applications/`
- `share/icons/*` → `~/.local/share/icons/`
- `share/systemd/user/*` → `~/.local/share/systemd/user/`

**Systemd user services** (`share/systemd/user/*.service`) receive special handling:
1. Linked to `~/.local/share/systemd/user/`
2. `systemctl --user daemon-reload` is run
3. Service is enabled with `systemctl --user enable`
4. Service is started with `systemctl --user start`

### metadata.yml Format

Defines how `src/` files are symlinked to `$HOME`:
```yaml
links:
  src:
    .: .config/tmux           # Link entire src/ dir to ~/.config/tmux
    bashrc.d/*: .config/bash/bashrc.d/  # Glob patterns supported

provides:
  bash-shell:                 # Declares provided capabilities

uses:
  ssh-client:
    constraint: required
```

### Relations (Cross-Module Integration)

When a module needs to provide configuration or integration for another module, it uses the `relations/` directory:

```
module-name/
├── relations/
│   └── other-module/                  # Files specific to other-module integration
│       └── config-file
└── hooks/
    └── other-module-relation-joined   # Executed when both modules are deployed
```

The `hooks/<other-module>-relation-joined` script is executed when both the current module and `<other-module>` are deployed. In these scripts, `$PWD` is set to the charm root directory.

Example (from `activity-memo/hooks/kitty-relation-joined`):

```bash
#!/bin/bash
set -eu

TARGET="$HOME/.config/kitty/open-actions.conf"
SOURCE="$PWD/relations/kitty/open-actions.conf"

ln -sf "$SOURCE" "$TARGET"
```

### Live-Profile System

The core dynamic environment system (`live-profile/`) injects environment variables at command execution time via compiled C shims. This enables runtime reconfiguration without restarting long-running processes (e.g., Emacs).

**Components:**
1. **Shim binaries** (`live-profile-shim.c`): Wrap commands like `ssh`, `git`, `ssh-add`
2. **Shell library** (`share/live-profile/lib.sh`): Scans `~/.config/live-profile/*.sh`
3. **Plugin scripts**: Each defines a `run()` function returning `VAR=value` pairs

Modules integrate via `relations/live-profile/` directories containing their environment scripts.

### Bash Initialization

`bash/src/bashrc.d/` uses numbered scripts for ordered loading:
- `00-09`: Terminal/early setup
- `10-19`: Environment variables
- `20-29`: Completion, options
- `30-39`: Prompt
- `80-89`: Editor, aliases
- `90-99`: External tools (nvm, pyenv, fzf, etc.)

Disable scripts by adding `.disable` extension. Per-host overrides go in `bashrc.d/host/${HOSTNAME}/`.

### Per-Host Configuration

Several modules support hostname-based variants:
- `alacritty/src/host/{kahn,wen}.toml`
- `bash/src/bashrc.d/host/${HOSTNAME}/`
- Live-profile shim compiled per-hostname

## Key Modules

- **live-profile**: Dynamic environment injection (central to ssh-agent, proxy, git)
- **bash**: Shell configuration with modular initialization
- **ssh-agent**: SSH key management with live-profile integration
- **0k-roadwarrior**: VPN/routing via sing-box
- **emacs**: Extensive org-mode based configuration
- **tmux**, **alacritty**, **kitty**: Terminal configurations

## External Dependencies

- `~/dev/sh/0k-cfg`: The configuration management tool
- `~/dev/sh/kal-shlib-common`: Shared shell libraries
- System packages: ripgrep, plocate
