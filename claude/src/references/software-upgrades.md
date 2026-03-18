# Software Upgrades

How to upgrade globally installed tools and user-built software.

## General Principles

- **Check current version first**: `<tool> --version` and `which <tool>`
- **Read changelogs** before upgrading (breaking changes, deprecations)
- **Verify after upgrade**: confirm version changed, tool still works
- Never upgrade mid-task — finish current work first

## Standard Global Package Upgrades

### npm global packages

```bash
npm update -g <package>
# or for a specific version:
npm install -g <package>@<version>
```

### Bun global packages

```bash
bun update -g <package>
```

### Cargo (Rust)

```bash
cargo install <crate>
```

### pip (Python)

```bash
pip install --upgrade <package>
```

## OpenCode + oh-my-opencode (Local Fork Setup)

Both OpenCode and oh-my-opencode are maintained as **local forks with
custom branches**. They must be updated together by rebasing local
branches onto the latest upstream.

### Architecture Overview

```
~/.local/bin/opencode  →  symlink to built binary
    ↓ (built from)
~/dev/ts/opencode/  (fork of anomalyco/opencode)
    ├── origin:   git@github.com:vaab/opencode.git
    └── upstream: https://github.com/anomalyco/opencode.git

~/.local/bin/oc  →  symlink to ~/.cfg-store/live-shared/opencode/bin/oc
    ↓ (wrapper that runs)
    opencode serve + opencode attach

~/.config/opencode/
    ├── opencode.json       ← runtime config (plugin list, permissions)
    ├── oh-my-opencode.json ← plugin config (agent models, hooks)
    ├── package.json        ← npm deps: @opencode-ai/plugin + oh-my-opencode
    ├── bun.lock
    └── node_modules/
        ├── @opencode-ai/plugin  ← from npm registry
        └── oh-my-opencode       ← file: link to local fork
              ↓ (linked from)
~/dev/ts/oh-my-opencode/  (fork of code-yeongyu/oh-my-opencode)
    ├── origin:   git@github.com:vaab/oh-my-opencode.git
    └── upstream: git@github.com:code-yeongyu/oh-my-opencode.git
```

### OpenCode Local Branches

The fork carries custom branches stacked on top of `dev` (upstream
default branch). These must be rebased in order when updating.

Current branch stack (rebase in this order):

1. `fix/cap-retry-after-delay` — cap Retry-After header delay
2. `fix/provider-connect-timeout` — add connect timeout (depends on 1)
3. `feat/runtime-model-fallback` — runtime model fallback (depends on 2)
4. `feat/stream-idle-timeout` — stream idle timeout (depends on 3, **currently deployed**)

The active branch (`feat/stream-idle-timeout`) contains all commits
from the branches below it. The binary is built from this branch.

To inspect what each branch adds on top of the previous:

```bash
cd ~/dev/ts/opencode
git log --oneline dev..fix/cap-retry-after-delay
git log --oneline fix/cap-retry-after-delay..fix/provider-connect-timeout
git log --oneline fix/provider-connect-timeout..feat/runtime-model-fallback
git log --oneline feat/runtime-model-fallback..feat/stream-idle-timeout
```

**Important**: After rebase, if a branch becomes empty (upstream
absorbed the fix), drop it from the stack and rebase the next branch
onto the one before it. Update this document accordingly.

### oh-my-opencode Local State

Currently tracking upstream `dev` with no local branches. The fork is
synced to upstream via `gh repo sync`.

### Plugin Dependency (`~/.config/opencode/package.json`)

```json
{
  "dependencies": {
    "@opencode-ai/plugin": "<version>",
    "oh-my-opencode": "file:/home/vaab/dev/ts/oh-my-opencode"
  }
}
```

- `@opencode-ai/plugin` is pinned to a **registry version** (not
  local) because the opencode monorepo uses workspace dependencies
  that cannot resolve outside its monorepo context.
- `oh-my-opencode` uses `file:` to point at the local fork clone.
  After rebuilding the fork, run `bun install` in
  `~/.config/opencode/` to pick up changes.
- The `@opencode-ai/plugin` version should be compatible with what
  oh-my-opencode expects. Check with:
  `grep '"@opencode-ai/plugin"' ~/dev/ts/oh-my-opencode/package.json`

### Full Upgrade Procedure

#### Step 1: Fetch upstream for both repos

```bash
cd ~/dev/ts/opencode
git fetch upstream

cd ~/dev/ts/oh-my-opencode
git fetch upstream
```

#### Step 2: Update oh-my-opencode

Since there are currently no local branches, this is a fast-forward:

```bash
cd ~/dev/ts/oh-my-opencode
git checkout dev
git rebase upstream/dev
# If local branches exist, rebase them in stack order here
bun install
bun run build
```

Then install into the config:

```bash
cd ~/.config/opencode
bun install
```

#### Step 3: Rebase OpenCode local branches

Rebase each branch in stack order onto the updated upstream. **Stop
and consult the user if any rebase has conflicts.**

```bash
cd ~/dev/ts/opencode

# Update dev to upstream
git checkout dev
git rebase upstream/dev

# Rebase stack in order — STOP ON CONFLICT
git checkout fix/cap-retry-after-delay
git rebase dev

git checkout fix/provider-connect-timeout
git rebase fix/cap-retry-after-delay

git checkout feat/runtime-model-fallback
git rebase fix/provider-connect-timeout

git checkout feat/stream-idle-timeout
git rebase feat/runtime-model-fallback
```

#### Step 4: Build and install OpenCode

```bash
cd ~/dev/ts/opencode
bun install
bun run --cwd packages/opencode script/build.ts --single
```

#### Step 5: Check `@opencode-ai/plugin` compatibility

```bash
# What oh-my-opencode needs (semver range):
grep '"@opencode-ai/plugin"' ~/dev/ts/oh-my-opencode/package.json

# What is currently installed:
cat ~/.config/opencode/node_modules/@opencode-ai/plugin/package.json | grep '"version"'

# If the installed version no longer satisfies the range, update
# package.json in ~/.config/opencode/ to a compatible registry version
# and run: bun install
```

#### Step 6: Verify

```bash
# Check binary
ls -la ~/.local/bin/opencode
opencode --version

# Check oh-my-opencode dist was rebuilt
ls -l ~/.config/opencode/node_modules/oh-my-opencode/dist/index.js

# Check plugin compatibility
cat ~/.config/opencode/node_modules/@opencode-ai/plugin/package.json | grep '"version"'
```

#### Step 7: Restart the server

The `oc` wrapper manages a persistent server in tmux. After updating:

```bash
# Kill the existing server (oc will restart it on next invocation)
tmux kill-session -t oc 2>/dev/null
# Or just kill the opencode serve process:
pkill -f "opencode serve"

# Next `oc` invocation will start a fresh server with updated code
```

### Troubleshooting

- **Wrong binary picked up**: Check `which opencode`. If nvm's version
  wins, ensure `npm uninstall -g opencode-ai` was run.
- **Build fails**: Ensure Bun ≥ 1.3 (`bun --version`). Run `bun install`
  again.
- **Stale binary**: The symlink points to the `dist/` directory. Rebuild
  with `--single` to update the binary in-place.
- **`bun install` fails in `~/.config/opencode/`**: Usually a version
  mismatch in `@opencode-ai/plugin`. Check that the version in
  `package.json` exists on the npm registry and is compatible with
  oh-my-opencode's requirements.
- **Skills not discovered after update**: The oh-my-opencode plugin
  needs `ctx.directory` to find project-level skills. This was fixed
  in upstream PR #1809 (2026-02-13). If skills are missing, ensure
  the installed oh-my-opencode includes this fix.
- **Rebase conflicts**: If a local branch conflicts during rebase,
  assess whether the upstream change obsoletes the local fix. If the
  upstream now includes equivalent functionality, drop the local
  branch from the stack and update this document.

### Development Mode (no build needed)

For quick iteration without building a binary:

```bash
cd ~/dev/ts/opencode
bun dev           # Run TUI from source
bun dev .         # Run against repo root
bun dev serve     # Headless API server
```
