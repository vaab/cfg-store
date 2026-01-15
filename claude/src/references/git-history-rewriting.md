# Git History Rewriting

Non-interactive workflows for rewriting commit history. Use only on
unpushed commits.

## Prerequisites

Before rewriting history:

```sh
# Verify commits are not pushed (no remote, or branch is ahead)
git remote -v
git status -sb  # Look for [ahead N]
```

## Line-Level Staging with Patches

To stage specific lines (not whole files or hunks), create a patch and
apply it to the index.

### Patch Format

```diff
--- a/path/to/file
+++ b/path/to/file
@@ -START,COUNT +START,COUNT @@
 context line (unchanged, starts with space)
-removed line (starts with minus)
+added line (starts with plus)
 context line
```

Rules:
- Path must match exactly (use ``git diff`` output as reference)
- Context lines must match the file exactly
- The @@ line numbers: ``-START,COUNT`` for old file, ``+START,COUNT`` for new
- COUNT includes context lines + changed lines in that block

### Applying Patches to Index

```sh
# Stage specific lines without modifying working tree
echo "$patch" | git apply --cached

# Or from a file
git apply --cached < patch_file
```

### Generating a Partial Patch

To extract specific changes from the current diff:

```sh
# Get full diff for reference
git diff path/to/file

# Create patch manually with only desired lines, or:
# Use git diff with line numbers to help identify sections
git diff --unified=3 path/to/file
```

## Fixup Workflow

When changes belong to older unpushed commits, use fixup commits and
autosquash rebase.

### Step 1: Identify Target Commits

```sh
# Find which commit introduced a specific line/pattern
git log --oneline -S "search string" -- path/to/file

# Or search in patch content
git log --oneline -p -- path/to/file | grep -B5 "pattern"
```

### Step 2: Create Fixup Commits

For each target commit, stage only the relevant changes and create a
fixup:

```sh
# Stage specific lines (see patch method above)
echo "$patch" | git apply --cached

# Create fixup commit (message auto-generated)
git commit --fixup=<target-commit-hash>
```

Repeat for each target commit needing fixes.

### Step 3: Stage and Commit New Changes

After all fixups are created, commit remaining changes normally:

```sh
git add <new-files>
git commit -m "new: [module] description"
```

### Step 4: Autosquash Rebase (Non-Interactive)

```sh
# Find the parent of the oldest fixup target
# If targets are abc123 and def456, and abc123 is older:
GIT_SEQUENCE_EDITOR=: git rebase -i --autosquash abc123^
```

The ``GIT_SEQUENCE_EDITOR=:`` uses ``:`` (no-op) as the editor, accepting
the auto-arranged sequence without interaction.

### Complete Example

```sh
# Scenario: file.txt has changes belonging to commit abc123 and new changes

# 1. Create patch for lines belonging to abc123
patch='--- a/file.txt
+++ b/file.txt
@@ -10,3 +10,3 @@
 context before
-old incorrect line
+fixed line
 context after'

# 2. Stage and create fixup
echo "$patch" | git apply --cached
git commit --fixup=abc123

# 3. Commit remaining changes
git add file.txt
git commit -m "new: [module] add new feature"

# 4. Rebase to squash fixup into target
GIT_SEQUENCE_EDITOR=: git rebase -i --autosquash abc123^
```

## Resetting Staged Changes

If a patch apply goes wrong:

```sh
# Unstage everything
git reset HEAD

# Unstage specific file
git reset HEAD -- path/to/file
```

## Verifying Results

After rebase:

```sh
# Check the rewritten commits
git log --oneline -5

# Verify content is correct
git show <commit-hash>

# Ensure working tree is clean
git status
```

## Troubleshooting

### Patch Does Not Apply

- Check line numbers in @@ header match current index state
- Ensure context lines match exactly (whitespace matters)
- Verify path is correct (``a/`` and ``b/`` prefixes)

### Rebase Conflicts

If conflicts occur during autosquash:

```sh
# Abort and try a different approach
git rebase --abort

# Or resolve conflicts manually
git add <resolved-files>
git rebase --continue
```

### Multiple Fixups to Same Commit

Works fine - create multiple fixup commits to the same target, they'll
all be squashed in order.
