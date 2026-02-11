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

## OpenCode (Special Case)

**OpenCode is built from local source**, not installed from npm.

- **Source**: `~/dev/ts/opencode` (monorepo, Bun workspace)
- **Core package**: `~/dev/ts/opencode/packages/opencode`
- **Binary**: `~/.local/bin/opencode` → symlink to built binary

### Upgrade Procedure

```bash
# 1. Pull latest source
cd ~/dev/ts/opencode
git pull

# 2. Install dependencies
bun install

# 3. Build for current platform only
bun run --cwd packages/opencode script/build.ts --single

# 4. Verify the symlink points to the right place
ls -la ~/.local/bin/opencode
# Should point to: ~/dev/ts/opencode/packages/opencode/dist/opencode-linux-x64/bin/opencode

# 5. Verify
opencode --version
```

### Initial Setup (if not yet switched from npm global)

```bash
# Build local binary
cd ~/dev/ts/opencode
bun install
bun run --cwd packages/opencode script/build.ts --single

# Symlink to PATH
ln -s ~/dev/ts/opencode/packages/opencode/dist/opencode-linux-x64/bin/opencode ~/.local/bin/opencode

# Remove the npm global so local takes precedence
npm uninstall -g opencode-ai

# Verify
which opencode   # Should show ~/.local/bin/opencode
opencode --version
```

### Troubleshooting

- **Wrong binary picked up**: Check `which opencode`. If nvm's version
  wins, ensure `npm uninstall -g opencode-ai` was run.
- **Build fails**: Ensure Bun ≥ 1.3 (`bun --version`). Run `bun install`
  again.
- **Stale binary**: The symlink points to the `dist/` directory. Rebuild
  with `--single` to update the binary in-place.

### Development Mode (no build needed)

For quick iteration without building a binary:

```bash
cd ~/dev/ts/opencode
bun dev           # Run TUI from source
bun dev .         # Run against repo root
bun dev serve     # Headless API server
```
