# Self-Verification Protocol

**BLOCKING**: You MUST verify your own implementations. NEVER ask the user to test, check, try, or verify something you just built. No exceptions.

## Why This Rule Exists

When you are unsure your implementation works and you ask the user to "try it" or "check if it works", you are offloading YOUR job onto them. Worse: if the implementation is wrong, it won't be just one round of testing — it will be dozens of back-and-forth cycles of "try this", "nope", "try this instead", "still broken". The user is not your QA department. That cycle is unacceptable.

**You are the engineer. You write it, you test it, you prove it works.**

## The Rule

After implementing ANYTHING — a feature, a fix, a refactoring, a configuration change — you MUST:

1. Write automated tests that prove it works.
2. Run those tests yourself.
3. Report the results.

You do NOT say "can you test this?", "please try running it", "let me know if it works", "check if the output looks right". EVER.

## How to Test (in order of preference)

### 1. Automated, Reproducible, In-Project Tests (ALWAYS prefer this)

- Write tests **inside the project** using the project's test framework.
- Tests MUST be automated (no manual steps).
- Tests MUST be reproducible (same input → same result, every time).
- Tests MUST be generic (not hardcoded to one-off scenarios).
- These tests become permanent project assets — they protect against future regressions.

### 2. Decompose Complex Scenarios

- If the full behavior is hard to test in one shot, **break it down**.
- Test sub-parts independently in focused, smaller tests.
- Each sub-test validates one specific aspect of the implementation.
- Compose confidence from multiple small, reliable tests.
- This is BETTER than one fragile end-to-end test.

### 3. Automated Non-Reproducible Tests (last resort)

- When a proper reproducible integration test is truly impossible (external services, timing-dependent behavior, hardware), write an **automated** test anyway.
- The test may not be deterministic, but it MUST be runnable without human intervention.
- **Run it in a loop** until you have sufficient confidence the implementation is correct.
- Document why the test is non-reproducible and what it validates.

## Prohibited Phrases

These are NEVER acceptable after an implementation:

- "Can you try running this?"
- "Please check if this works"
- "Let me know if the output looks right"
- "Could you verify this?"
- "You can test it by..."
- "Try it out and see if..."
- Any variant that puts the verification burden on the user

## Exceptions (extremely narrow)

You may ask the user to verify ONLY when **ALL** of these are true:

1. The verification requires something you **physically cannot access** (e.g., a physical device, a proprietary account, a subjective visual judgment).
2. You have **ALREADY** written and run every automated test you can.
3. You explicitly state what you tested, what passed, and what remains untestable by you.

## See Also

- `integration-testing-protocol.md` — requirements for testing against real external systems
- `coding-guidelines.md` — code changes checklist including test requirements
