# Diagnostic Protocol

When a user reports a bug, unexpected behavior, or asks you to fix
something — or when you discover a bug yourself while working on
another task: **STOP. Do not fix anything yet.**

## The Rule

You are forbidden from applying a fix until you have:

1. Understood what is actually happening (with evidence).
2. Formed a hypothesis for _why_ it is happening.
3. Validated that hypothesis with technical proof.
4. Presented your findings to the user and obtained approval to proceed.

## Delegate Aggressively

Diagnosis is context-heavy. The more you read, run, and analyze in
your own context, the more you risk context rot — losing track of
earlier findings, drifting from the original symptom, or rushing to
a fix just to "wrap up."

**Use sub-agents for every substantial diagnostic step.** Stay the
orchestrator: dispatch focused tasks, collect terse reports, synthesize.

| Phase          | Delegate what                                       | Agent type                               |
| -------------- | --------------------------------------------------- | ---------------------------------------- |
| **Evidence**   | Code search, pattern discovery, git history         | `explore` (background)                   |
| **Evidence**   | Reproducing the bug, running with debug logs        | `task` (category appropriate to project) |
| **Evidence**   | External docs, library behavior, known issues       | `librarian` (background)                 |
| **Hypothesis** | Analyze collected evidence, form causal explanation | `oracle` or `task`                       |
| **Validation** | Write and run a targeted test to confirm/deny       | `task` (category appropriate to project) |

**Each sub-agent prompt must include**:

1. The exact symptom reported by the user
2. The specific evidence collected so far
3. What you need back — a terse, factual report (not a fix)

**Each sub-agent report you collect** becomes a building block for
your Phase 3 presentation. You synthesize, you do not re-derive.

## Phase 1: Gather Evidence

Before forming any opinion, collect hard data, you can:

- **Read the relevant code** — note exact file paths and line numbers.
- **Reproduce the problem** — run the software, trigger the bug,
  observe the actual output or behavior.
- **Add instrumentation** — insert logging, debug prints, or
  assertions to expose internal state. Remove them after diagnosis.
- **Vary conditions** — run with different inputs, configurations, or
  environments to isolate what triggers the issue.
- **Check history** — `git log`, `git blame`, recent changes that
  may have introduced the regression.

Collect raw technical data. Do not guess.

**Delegate**: fire `explore` agents in parallel to search the
codebase (error paths, related modules, recent changes). Fire
`librarian` if external libraries are involved. Collect their
reports before forming any opinion.

### Assess the Blast Radius

Before adding any instrumentation or making diagnostic changes,
determine what you are working with:

| Environment          | Examples                                   | Implication                                       |
| -------------------- | ------------------------------------------ | ------------------------------------------------- |
| **Isolated binary**  | Local CLI tool, dev build you run yourself | Low risk — instrument freely                      |
| **Local service**    | Dev server on localhost, local DB          | Moderate — check what else connects to it         |
| **Shared config**    | Dotfiles, system configs, tool settings    | Moderate — changes affect other tools/sessions    |
| **Multi-user / VPS** | Shared server, staging with other users    | High — your debug changes may affect others       |
| **Production**       | Live customer-facing service, prod DB      | Critical — do not touch without explicit approval |

**Ask yourself**: if my diagnostic change goes wrong or lingers,
what is the worst that happens? Just my terminal breaks? Other
users see errors? Customer data is affected?

If you are unsure about the environment's sensitivity, **ask the
user** before inserting instrumentation. A one-line question now
prevents a production incident later.

### Protect the Environment

- **Use isolation**: run tests, debug builds, or scratch branches
  — not the user's live/production setup.
- **Never modify production state** (databases, config files,
  deployed artifacts) as part of diagnosis.
- **If you must touch shared resources**, warn the user first and
  get explicit approval.
- **Scale caution to blast radius**: the higher the impact row in
  the table above, the more you must justify and get approval for
  every diagnostic step — not just the fix.

## Phase 2: Form and Prove a Hypothesis — No Fix Yet

**Delegate hypothesis formation.** Send the collected evidence to
an `oracle` or a `task` agent with a clear prompt: "Given this
evidence, what is the most likely cause? Rank by likelihood. Cite
specific evidence for each." Do not form the hypothesis yourself
from a bloated context — let a fresh agent analyze the data.

Once you have a causal explanation from the sub-agent:

- **Be specific**: "Line 42 of `handler.rs` reads `config.timeout`
  which is `None` when the env var is unset" — not "the config
  might be wrong."
- **Show your proof**: log output, stack traces, test results,
  variable values, line numbers. If you cannot point to concrete
  evidence, you do not have a hypothesis — you have a guess.
- **If the problem is complex**, consolidate your hypothesis before
  presenting it. Do not shotgun multiple guesses at the user —
  narrow down to one well-supported explanation (or a small ranked
  list, each with its own evidence).

**NEVER modify behavior before approval.** "Hypothesis" means
analysis, not experimentation. You may read code, add logging, run
the existing software, and inspect output. You may NOT apply
speculative patches or "try something and see." All of that happens
only after Phase 3 approval.

### After Approval: Track What You Try

Once the user approves a fix direction, keep a written log of each
attempt and its outcome. Create a `fix-xxxxxxxx.md` using a short
label for the issue in the project root:

```markdown
# Diagnosis: <short description>

## Hypothesis

<what you believe is wrong and why>

## Attempts

### Attempt 1 — <date or sequence>

- Change: <what you modified>
- Result: <what happened — pass/fail, output, observations>

### Attempt 2 — ...
```

- Update this file after each attempt.
- **Delete the file** once the issue is resolved and the fix is
  verified.
- This log prevents repeating failed approaches and gives the user
  visibility into the process.

### What Counts as Proof

| Acceptable                                   | Not acceptable            |
| -------------------------------------------- | ------------------------- |
| Log output showing the faulty value          | "I think it might be..."  |
| A minimal reproduction that triggers the bug | "This looks suspicious"   |
| A test that fails, proving the defect        | "In my experience..."     |
| `git blame` showing the introducing commit   | "It's probably this line" |
| Debug output from added instrumentation      | Gut feeling               |

### Label Every Claim (Epistemic Discipline)

When presenting findings — whether mid-investigation or final — every
non-trivial claim MUST be prefixed with its evidence type. This is a
language discipline: treat it as non-optional formatting for diagnostic
output.

| Prefix         | Meaning                                                       | Example                                                                                           |
| -------------- | ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| `[OBSERVED]`   | Direct tool output, file contents, log line, DB row           | `[OBSERVED] auth.log line 25: event="refresh_needed"`                                             |
| `[INFERRED]`   | Derived from pattern matching, source reading, or correlation | `[INFERRED] Plugin was likely reloaded at 22:49:40 (based on presence of plugin_init in the log)` |
| `[HYPOTHESIS]` | Speculative, not yet verified                                 | `[HYPOTHESIS] A race between sync timer and refresh may clobber the cache`                        |
| `[QUESTION]`   | Something I don't know and should find out                    | `[QUESTION] Is the plugin re-initialized per session or once per server run?`                     |

**Forbidden declarative phrases** when you only have indirect evidence:

- "I proved X"
- "X is resolved"
- "The cause is Y"
- "X happened at time T" (unless T was read directly from a log line)

**Required instead**:

- `[INFERRED] X appears resolved based on <specific signal>; still unverified because <gap>`
- `[OBSERVED] Log line N at T: "<exact text>"`
- `[HYPOTHESIS] Likely cause is Y because <reasoning>; not yet verified by <method>`

### The Timing-Attribution Trap

Before attributing any wall-clock anomaly (gap, delay, speed-up) to a
specific cause, enumerate alternative explanations explicitly:

- Is the LLM session itself suspended/paused between turns? (wall-clock
  advances independently of the LLM's internal sense of elapsed time)
- Is another process writing to the same log/clock?
- Is clock skew between host and container/subprocess a factor?
- Was the buffer rotated, truncated, or the file recreated?
- Is there a periodic task (cron, systemd timer, background thread)
  that fires at the interval you observed?

Commit to one explanation only after eliminating the alternatives — and
even then, label it `[INFERRED]`, not `[OBSERVED]`.

## Phase 3: Present to User — No Code Changes Without Approval

**This is the checkpoint. You do not pass it without user approval.**

Present to the user:

1. **What is happening** — the observable symptom, with evidence.
2. **Why it is happening** — your hypothesis, with the proof that
   supports it.
3. **What you tried** — the diagnostic steps you took and what
   each revealed.
4. **What you propose to fix** — the specific change, and why you
   believe it addresses the root cause (not just the symptom).

Wait for the user's go-ahead before making any code change.

### When to Present (even mid-diagnosis)

- If you need to modify the user's code to add instrumentation beyond
  trivial logging — ask first.
- If your diagnosis is taking long and you have partial findings —
  share them. Do not disappear into a rabbit hole.
- If you discover the problem is different from what was reported —
  flag it immediately.

## Prohibited Behaviors

| Behavior                          | What it looks like                                                    | Do this instead                                                          |
| --------------------------------- | --------------------------------------------------------------------- | ------------------------------------------------------------------------ |
| **Fix on impulse**                | See symptom, change code immediately                                  | Collect evidence first (Phase 1)                                         |
| **Shallow hypothesis**            | "I see this, it must be that"                                         | Prove it with logs, traces, or a failing test                            |
| **Silent experimentation**        | Try fixes without telling the user                                    | Log every attempt, share findings                                        |
| **Fix without explaining cause**  | "I fixed it" (no diagnosis)                                           | Present what was wrong AND why the fix works                             |
| **Guess under pressure**          | Speculate when you don't know                                         | Say "I don't know yet" and propose a diagnostic step                     |
| **Mutate state during diagnosis** | Accept/reject pending items, call APIs that change state, delete logs | Read-only observation only; state changes require explicit user approval |

## No Exceptions

This protocol applies **every time** something needs to be fixed,
regardless of how the request is framed:

- "It's not working, fix it" → diagnose, present, get approval.
- "Go fix the timeout bug" → diagnose, present, get approval.
- "This is broken" → diagnose, present, get approval.
- Even if the fix seems obvious to you → diagnose, present, get
  approval.

Make the user understand what went wrong and why your fix addresses
it. A fix without a diagnosis is not a fix — it is a gamble.

## Example: Correct vs Incorrect Flow

**User says**: "The scheduler isn't firing my cron task."

### WRONG (fix on impulse)

1. Look at cron parsing code
2. See a suspicious off-by-one in month handling
3. Change `month - 1` to `month`
4. Tell user "I fixed a bug in the cron parser"

_Problem_: no evidence the off-by-one was the cause. Could be a
timezone issue, a `last_run` comparison bug, or the task being
disabled.

### RIGHT (diagnostic protocol)

1. **Evidence**: Read the task from `tasks.json` — `enabled: true`,
   cron: `0 0 8 * * * *`. Check `last_run` — it's `null`.
2. **Reproduce**: Add a debug log in the scheduler tick loop. Run
   the scheduler. Observe: "Task X: next_fire=2026-02-23T08:00:00,
   now=2026-02-22T15:00:00, skipping."
3. **Hypothesis**: The task is not due yet — the cron expression
   means 8 AM daily, and it's 3 PM today with `last_run: null`,
   so next fire is tomorrow 8 AM. The scheduler is correct.
4. **Present**: "The task is working correctly. It fires at 8 AM
   daily and hasn't reached its first fire time yet. The cron
   expression `0 0 8 * * * *` means 08:00:00 every day. Do you
   want me to change the schedule?"

## See Also

- `self-verification.md` — after a fix is approved and applied,
  you must verify it works yourself
- `coding-guidelines.md` — general code quality expectations

---

**Diagnose → prove → present → get approval → THEN fix. Every time. No exceptions.**
