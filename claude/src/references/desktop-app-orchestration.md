# Desktop App Orchestration

How to interact with GUI desktop applications from the terminal.

## Tools

- `winxy` — coordinate conversion + screenshot + click (see below)
- `xdotool` — window management and input simulation
- `wmctrl` — window discovery
- `xwd` — screenshot by window ID (no focus required)

## General Workflow

1. **Launch** the app (with any required flags)
2. **Find** its window via `winxy` or `xdotool search`
3. **Screenshot** by window ID: `winxy --screenshot /tmp/app.png "AppName"`
4. **Analyze** with `look_at` vision tool (get % coordinates)
5. **Click** at position: `winxy --click "AppName" <x_pct> <y_pct>`
6. **Interact** via `xdotool key`/`xdotool type`

### Window Discovery

```bash
# Find visible window by title pattern (filters tiny helper windows)
winxy --screenshot /tmp/app.png "AppName"

# Or manually:
wmctrl -l -G | grep -i "AppName"
xdotool search --onlyvisible --name "AppName"
```

### Screenshot (by window ID — focus independent)

**Always use `winxy --screenshot`** or `xwd -id` to capture by
window ID.  Never use `gnome-screenshot -w` — it captures whichever
window has focus, which is unreliable during automation.

```bash
# Preferred: captures by window ID, auto-resizes for vision
winxy --screenshot /tmp/app.png "AppName"

# Manual equivalent (requires ImageMagick):
WID=$(xdotool search --onlyvisible --name "AppName" | head -1)
xwd -id "$WID" -silent | convert xwd:- -resize 1920x1920\> /tmp/app.png
```

### Clicking (winxy)

`winxy` converts screenshot-relative percentage coordinates to
absolute screen coordinates, accounting for HiDPI scaling.

```bash
# Dry-run: print absolute coordinates
winxy "AppName" 50 30

# Click at 50% x, 30% y of the window
winxy --click "AppName" 50 30

# Verbose: show coordinate math
winxy --click -v "AppName" 50 30
```

### Coordinate Model (X11 + GNOME HiDPI)

`xdotool getwindowgeometry --shell` returns:
- **X, Y** in logical (scaled) coordinates — same space as `mousemove`
- **WIDTH, HEIGHT** in compositor pixels (unscaled)

`xwd -id` captures in compositor pixels too, but may differ from
xdotool's WIDTH/HEIGHT by a small border (e.g., 40px for Firefox).
`winxy` probes the actual `xwd` dimensions and uses those as the
percentage base, since they match what vision sees in the screenshot.

Scale factor (from `gsettings scaling-factor`) converts pixel offsets
to logical offsets:

```
click = origin_XY + round(pct/100 * xwd_dimension / scale)
```

`winxy` handles this automatically.  Override scale with
`WINXY_SCALE=<int>` if auto-detection fails.

**Note**: for maximized windows whose frame extends behind the GNOME
panel, `xwd` captures content including the hidden portion.  The
percentages from vision still map correctly — the formula accounts
for the full screenshot dimensions.

### Input Simulation

```bash
# Activate window first (winxy --click does this automatically)
wmctrl -i -a <wid>
sleep 0.5

# Keyboard shortcuts
xdotool key ctrl+f
sleep 0.3

# Type text (with delay between keystrokes for reliability)
xdotool type --delay 30 "search query"

# Press Enter
xdotool key Return
```

### winxy Location

Project: `~/dev/python/winxy/winxy`
Symlink: `~/.local/bin/winxy`

## Firefox

### Launch / Binary

Firefox is installed via snap (`/snap/firefox/*/usr/lib/firefox/firefox`).
The CLI wrapper is at `/usr/bin/firefox`.

### Open a URL (new tab in running instance)

```bash
firefox https://example.com &>/dev/null & disown
```

If Firefox is already running, this opens the URL in a new tab in
the existing window. No special flags needed.

### Window Identification

Firefox registers with `wmctrl` with a title ending in
`— Mozilla Firefox`:

```bash
wmctrl -l | grep -i firefox
```

### Tab Navigation

```bash
WID=$(wmctrl -l | grep -i firefox | awk '{print $1}')
wmctrl -i -a "$WID"
sleep 0.5

# Next / previous tab
xdotool key ctrl+Tab
xdotool key ctrl+shift+Tab

# Go to tab by position (1-8, 9 = last)
xdotool key ctrl+1

# New empty tab
xdotool key ctrl+t

# Close current tab
xdotool key ctrl+w

# Address bar (type a URL or search)
xdotool key ctrl+l
sleep 0.3
xdotool type --delay 30 "https://example.com"
xdotool key Return
```

### Typical Workflow

```bash
# 1. Open URL (launches or reuses running instance)
firefox https://chatgpt.com &>/dev/null & disown
sleep 3

# 2. Screenshot by window ID (no focus needed)
winxy --screenshot /tmp/firefox.png Firefox

# 3. Analyze with look_at, then click
winxy --click Firefox 6 46
```

## Signal Desktop

### Launch Command

Signal requires `--password-store=gnome-libsecret` when launched from
a non-desktop shell (e.g., tmux, SSH, Claude Code terminal). Without
it, Electron detects `basic_text` instead of `gnome_libsecret` as the
safe storage backend, causing a database decryption failure.

```bash
/opt/Signal/signal-desktop --password-store=gnome-libsecret &>/dev/null & disown
```

**Error without the flag:**
```
Detected change in safeStorage backend, can't decrypt DB key
(previous: gnome_libsecret, current: basic_text)
```

### Window Identification

Signal registers with `wmctrl` under the title `Signal`:

```bash
wmctrl -l -G | grep "Signal"
```

### Interaction Notes

- **Search chats**: `Ctrl+F` may not reliably trigger Signal's search
  from `xdotool`. The chat list is visible on the left panel — if the
  target chat is recent, it may already be visible.
- **Message input**: Click the message field at the bottom, then
  `xdotool type` to draft text.
- **Do NOT send**: For read/draft workflows, type but do not press
  Enter in the message field.

### Typical Workflow

```bash
# 1. Launch
/opt/Signal/signal-desktop --password-store=gnome-libsecret &>/dev/null & disown

# 2. Wait for window
for i in $(seq 1 15); do
  sleep 1
  winxy --screenshot /tmp/signal.png Signal 2>/dev/null && break
done

# 3. Analyze with look_at, then click on a chat
winxy --click Signal 50 30
```
