# Architectural Consolidation

Before adding any new concept (function, class, file, module,
abstraction) to an existing project, **map the new requirement onto
the existing conceptual structure**. Consolidate over append.

A codebase is not a list of features. It is a model of overlapping
responsibilities. Every change must leave that model **at least as
clear** as it was before. If your change makes the model fuzzier,
duplicates a responsibility, or introduces a parallel concept doing
half of what an existing one already does — **the change is wrong,
even if the feature works.**

This protocol runs **before** the implementation plan you present per
`coding-guidelines.md`. The architectural map IS part of the plan.

## Step 1: Name the Concept

Before searching anything, write down in plain language:

- **What concept** is the user introducing or modifying?
- **What responsibility** does it carry? (one sentence, one verb)
- **What does it NOT do?** (the boundary)

If you cannot name it cleanly in one sentence, the concept is not
yet clear in your head. Refine before continuing.

## Step 2: Find Adjacent Concepts

Search the codebase along three axes. Do not skip any.

| Axis              | What to look for                                                         | Tools                                |
| ----------------- | ------------------------------------------------------------------------ | ------------------------------------ |
| **By name**       | Existing names that overlap (synonyms, near-synonyms, same noun)         | `grep -r`, semantic search           |
| **By responsibility** | Modules/functions that already own part of this responsibility       | Read code, ask `explore` agent       |
| **By layer**      | Neighbors at the same architectural layer (same dir, same role, same suffix) | List sibling files, read 2-3        |

If you find candidates, read them. **You are forbidden from
proposing a new concept until you can describe what each adjacent
existing concept does and how it relates to your new one.**

## Step 3: Apply the Four Diagnostic Questions

For each adjacent concept you found, answer:

1. **Overlap test** — Is my new code *braided into* this concept's
   responsibility, or *placed alongside* it doing half of what it
   already does? *(Hickey: complect vs. compose; Fowler: Shotgun
   Surgery, Parallel Inheritance)*
   - Red flag: "two retry loops", "two error handlers", "two config
     systems", `rateLimiter.py` next to `rateLimiterEnhanced.py`.

2. **Change-axis test** — When this new behavior changes in the
   future, will it change for the *same reason* as some existing
   code? If yes, they belong together. *(Martin's actual SRP: gather
   things that change for the same reasons.)*

3. **Adjacent-layer test** — Does my new module sit at the same
   layer as an existing one with a similar abstraction? Red flag.
   *(Ousterhout: "adjacent layers with similar abstractions" =
   suspect the decomposition.)*

4. **Naming test** — Does this concept already have a name in the
   codebase's vocabulary? Inventing a new name for an existing idea
   fractures the ubiquitous language. *(Evans: ubiquitous language.)*

If **any** answer flags a problem, do NOT proceed to "introduce a
new concept". Pick one of the outcomes in Step 4.

## Step 4: Choose One Outcome

Exactly one of these. Name the outcome explicitly in your plan.

### Outcome A — Extend an existing concept *(default, prefer this)*

Deepen the existing module: add a method, generalize a parameter,
broaden a type. The existing concept absorbs the new behavior
because it is the natural owner.

**Example.** User asks: "add JWT auth on top of our existing
session auth." Existing: `AuthMiddleware` handles session cookies.
Outcome: extend `AuthMiddleware` to recognize both schemes via a
strategy pattern. **Wrong**: create a new `JwtAuthMiddleware`
parallel to `AuthMiddleware`.

### Outcome B — Redraw responsibility, then plug in

A small piece of an existing concept's job moves to a new home, and
the new feature plugs into that redrawn boundary. The total concept
count may stay the same or even decrease. Always document **what
moved and why**.

**Example.** User asks: "add rate-limiting to the API." Existing:
`HttpClient` does retries AND timeouts AND nothing else;
`RateLimiter` does not exist. Outcome: extract a `RequestPolicy`
concept that owns retries+timeouts+rate-limit, and reduce
`HttpClient` to "send one HTTP request". **Wrong**: glue rate-limit
logic into `HttpClient` next to retries.

### Outcome C — Introduce a new concept *with named relationships*

Only if the new concept has a clear, single responsibility that
**does not overlap any existing one**, and you can name how it
relates to its neighbors ("X uses Y", "X is composed of Y", "X
replaces Y").

**Example.** User asks: "add a metrics sink for Prometheus." No
existing concept owns metric export. Outcome: new `MetricsSink`
concept, *related to* the existing `Logger` (sibling: both are
observability outputs) but with a separate, non-overlapping
responsibility (numeric time-series, not text events).

### Outcome D — STOP and surface to user *(mandatory escalation)*

If the only way to add the feature is a parallel/overlapping
concept (the four diagnostic questions all flag red), do NOT
proceed. Surface to the user using the template in §5.

## Step 5: Mandatory Escalation Template

Use this exact structure when you must surface. **Do not ask vague
questions.** Vague questions waste the user's time and signal you
have not done the mapping.

```
I can't add **<feature>** without breaking architectural integrity.
Here is the conflict I see:

- Existing concept **A** at `path/to/a.ext` already does **<part>**.
- Existing concept **B** at `path/to/b.ext` already does **<part>**.
- The new requirement needs **<X>**, which overlaps with both.

Three options I can see:

1. **Extend A** — shift `<piece>` from B into A; new feature plugs
   into A. Cost: touches B (~N lines) beyond your request.
2. **Introduce concept C** with responsibility `<Z>`, removing
   `<piece>` from both A and B. Cost: 3 files touched, ~M lines.
3. **Accept duplication** — add a third place doing `<part>`. I do
   not recommend this; it creates a Divergent Change risk.

I recommend **option N** because **<reason>**. Proceed with that,
choose another, or rethink the requirement?
```

## Out-of-Scope Refactoring Rule

Consolidation often requires touching code beyond the user's
request (Outcomes B and C). When this happens:

- **Always present the proposal first**, including a precise list
  of files/concepts touched and an estimate of collateral lines.
- **Wait for explicit consent** before any out-of-scope edit.
- Never refactor silently, even "while you're in there".
- This rule does NOT override `coding-guidelines.md`'s "plan first".
  It composes with it: the consolidation map IS your plan.

## Forbidden Behaviors

| Behavior                                                                            | Why it's forbidden                       |
| ----------------------------------------------------------------------------------- | ---------------------------------------- |
| Adding a file/class/function whose name is a near-synonym of an existing one        | Fractures vocabulary; duplicates concept |
| `Manager`/`Handler`/`Service`/`Controller` for the same noun as an existing concept | Same as above; classic LLM tell          |
| `XSimple.py`, `XEnhanced.py`, `XV2.py` next to existing `X.py`                      | "Debugging Residue" — orphaned variants  |
| Implementing a new code path that does "the existing thing plus one more case"      | Generalize the existing one instead      |
| Silent acceptance of duplicated responsibility ("I'll just add another retry here") | Compounding maintenance debt             |
| Refactoring scope creep — using consolidation as license to rewrite unrelated code  | Violates surgical-change discipline      |
| Inventing a new vocabulary word for a concept the codebase already names            | Fractures ubiquitous language            |

## Interaction With Other Rules

- **Runs before** `coding-guidelines.md`'s plan presentation. The
  architectural map IS part of the plan you present.
- **Runs after** `pre-work-checklist.md` (clean git first) and
  `diagnostic-protocol.md` (for bugs: diagnose, then map, then plan).
- **Composes with** `self-verification.md`: a verified change that
  introduces architectural drift is still a failed change. Working
  ≠ correct.
- **Composes with** `agents-md-convention.md`: project AGENTS.md
  files inherit this rule via the global CLAUDE.md trigger; they
  should not duplicate it.

## See Also (Source Material)

The principles above are distilled from:

- Brooks, *The Mythical Man-Month* (1975) — conceptual integrity:
  "better to omit features than to dilute the design philosophy".
- Parnas, "On the Criteria To Be Used in Decomposing Systems into
  Modules" (CACM 1972) — modules hide design decisions, not
  flowchart steps.
- Hickey, "Simple Made Easy" (Strange Loop 2011) — *complect* vs.
  *compose*; intertwined things must be considered together.
- Ousterhout, *A Philosophy of Software Design* (2018) — deep
  modules; "adjacent layers with similar abstractions" red flag;
  pass-through methods reveal confused responsibility.
- Martin, "The Single Responsibility Principle" (cleancoder.com,
  2014) — gather things that change for the *same reasons*.
- Fowler, *Refactoring* (1999/2018) — Divergent Change, Shotgun
  Surgery, Parallel Inheritance Hierarchies.
- Evans, *Domain-Driven Design* (2003) — ubiquitous language;
  bounded contexts must be internally consistent.
- Karpathy / forrestchang `andrej-karpathy-skills/CLAUDE.md` —
  "Surgical Changes": every changed line traces to user's request.
- Montes, "Lint Against the Machine" (2026) — "Debugging Residue":
  agents create variant files instead of fixing in place.
- OpenAI, "Harness Engineering" (2026) — Codex *will* replicate
  patterns including bad ones; mechanical enforcement (linters,
  structural tests) is the only durable fix.

---

**Map first. Consolidate over append. Surface conflicts with three
named options, never with vague questions. Every time. No exceptions.**
