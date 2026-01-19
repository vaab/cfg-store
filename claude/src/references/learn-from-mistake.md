# Learn From Mistake

A structured workflow for analyzing mistakes and implementing preventive measures.

## When to Use

When a mistake is identified (by user feedback or self-detection), follow this
workflow to prevent recurrence.

## Workflow

### Step 1: Analyze the Issue (Subagent)

Launch an **explore** subagent to identify what went wrong.

**Subagent prompt template:**
```
Analyze this mistake that occurred:

Context: [describe the situation]
What happened: [describe the mistake]
User feedback: [if any]

Tasks:
1. Identify specifically what was wrong
2. If unclear, list possible interpretations with your confidence level
3. Explain the expected correct behavior
4. Return a clear problem statement

If the issue is ambiguous, say so and list guesses ranked by likelihood.
```

**If unclear after subagent analysis:**
- Present guesses to user
- Ask user to clarify what was wrong
- Re-run analysis with user input

### Step 2: Root Cause Analysis (Subagent)

Once the issue is understood, launch an **explore** subagent to find causes.

**Subagent prompt template:**
```
Find root causes for this mistake:

Problem statement: [from Step 1]

Tasks:
1. List all possible causes (behavioral, procedural, environmental)
2. Filter to causes we have LEVERAGE over (can change instructions, add checks, etc.)
3. For each actionable cause, explain the mechanism (why did it lead to the mistake?)
4. Rank causes by:
   - Likelihood of being the true cause
   - Ease of addressing
   - Impact if fixed

Return a structured report with actionable causes only.
```

### Step 3: Generate Solutions (Subagent)

Launch an **oracle** subagent to generate improvement suggestions.

**Subagent prompt template:**
```
Generate solutions to prevent this mistake from recurring:

Problem statement: [from Step 1]
Root causes: [from Step 2]

Consider these solution types:
- Adding a BLOCKING instruction in CLAUDE.md or reference files
- Adding a checklist item to an existing workflow
- Creating a new reference file with guidelines
- Adding a pre-action verification step
- Modifying an existing instruction to be clearer

For each proposed solution, evaluate against these criteria:
- Specific and actionable (not vague "be more careful")
- Targeted at the root cause, not symptoms
- Minimal overhead (don't add friction for rare cases)
- Testable (can verify the fix works)

Return a report with:
1. List of proposed solutions with rationale
2. For each solution: where to implement, what to change
3. Expected effectiveness rating (high/medium/low)
4. Any trade-offs or downsides
```

### Step 4: Assess and Present

Review the solutions and present to user:

1. Re-read the root cause report and proposed solutions
2. Assess each solution for:
   - Likelihood to prevent recurrence
   - Side effects or overhead
   - Consistency with existing instructions
3. Rank solutions by effectiveness
4. Present top 1-3 recommendations to user with rationale

**Format:**
```
## Proposed Fix for: [problem summary]

**Root cause**: [brief description]

**Recommended solution(s):**

1. [Solution] - [why this helps]
   - Where: [file to modify]
   - Change: [specific change]

2. [Alternative if applicable]

**Trade-offs**: [any downsides]

Approve which solution(s) to implement?
```

### Step 5: Implement Approved Changes

After user approval:

1. Make the approved changes to instruction files
2. Verify the changes are syntactically correct
3. Summarize what was changed
4. Commit changes to cfg-store repo (don't push without explicit request)

## Examples of Preventable Mistakes

| Mistake | Root Cause | Fix |
|---------|------------|-----|
| Pushed without permission | Didn't read push convention | Add BLOCKING directive |
| Wrong commit format | Didn't read commit conventions | Add BLOCKING directive |
| Assumed version number | Didn't ask for release decisions | Add to release-guidelines |
| Missed required step | Instruction not prominent enough | Elevate to BLOCKING |

## Anti-Patterns

- Adding vague instructions ("be more careful")
- Adding excessive overhead for rare mistakes
- Blaming external factors when internal process could prevent issue
- Not actually implementing the fix after analysis
