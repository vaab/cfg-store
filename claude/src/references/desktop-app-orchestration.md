# Desktop App Orchestration

How to interact with GUI desktop applications from the terminal. Use
`xdotool` for window management and input simulation, `wmctrl` for
window discovery, and `gnome-screenshot` + PIL for visual feedback.

## General Workflow

1. **Launch** the app (with any required flags)
2. **Find** its window via `wmctrl -l` or `xdotool search`
3. **Activate** the window with `wmctrl -i -a <wid>`
4. **Interact** via `xdotool key`/`xdotool type`
5. **Capture** with `gnome-screenshot -f` + PIL crop to window bounds
6. **Read** the screenshot with `look_at` vision tool

### Window Discovery

```bash
# List all windows with geometry
wmctrl -l -G

# Find by title
wmctrl -l | grep -i "AppName"

# Get window geometry
xdotool getwindowgeometry <wid>
```

### Screenshot on Multi-Monitor

The user has a multi-monitor setup (11520×4320 total). Full
screenshots are too large for vision. Always crop to the target
window:

```python
from PIL import Image
import subprocess

img = Image.open("/tmp/screenshot.png")
# Parse wmctrl -l -G for x, y, w, h
crop = img.crop((x, y, x + w, y + h))
# Resize if needed (keep under ~1920×1080 for vision)
ratio = min(1920 / w, 1080 / h, 1.0)
crop.resize((int(w * ratio), int(h * ratio))).save("/tmp/app_view.png")
```

### Input Simulation

```bash
# Activate window first
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
  WID=$(wmctrl -l | grep "Signal" | awk '{print $1}')
  [ -n "$WID" ] && break
done

# 3. Activate and screenshot
wmctrl -i -a "$WID"
sleep 1
gnome-screenshot -f /tmp/signal.png

# 4. Crop to Signal window (parse wmctrl -l -G for coordinates)
# 5. Analyze with look_at tool
```
