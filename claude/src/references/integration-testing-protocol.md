# Integration Testing Protocol

**BLOCKING**: This protocol MUST be followed before any commit involving external system interactions (Hardware, APIs, Filesystem, CLI). Failure to verify on real systems is a violation of quality standards.

## 1. Definition & Scope

### Unit Test
- **Scope**: Isolated logic, single function, or module.
- **Dependencies**: Mocks, stubs, or in-memory equivalents.
- **Goal**: Verify algorithmic correctness and edge cases.
- **Constraint**: Must be fast and deterministic.

### Integration Test
- **Scope**: Interaction between two or more systems (e.g., App <-> Audio Driver).
- **Dependencies**: Real hardware, real network APIs, real filesystem state.
- **Goal**: Verify that assumptions made during development (and in mocks) hold true in the real world.
- **Requirement**: "Run tests" means executing **BOTH** Unit and Integration tests.

## 2. Core Requirements

- **Real World Verification**: If code interacts with an external dependency, it MUST be tested against that dependency. Mock-only verification is insufficient for production readiness.
- **Boundary Exhaustion**: Enumerate and test all supported formats, parameters, and states.
- **Escalation Protocol**:
  - If a real-world test reveals an unsupported format or missing parameter:
    1. **HALT** the commit.
    2. Document the limitation in the code (e.g., ``TODO`` or explicit error).
    3. Implement a fallback or a graceful failure.
    4. Notify the orchestrator/user before proceeding.

## 3. Per-Domain Guidelines

### Audio & Hardware
- **Hardware**: Must test with actual audio devices (Input/Output).
- **Formats**: Enumerate supported sample rates (44.1kHz, 48kHz) and formats (``f32``, ``i16``).
- **Example**: ``CpalCapture`` must be tested with a physical microphone, not just a stream mock.

### API & Network
- **Parameters**: Verify all "optional" parameters against the real API documentation and live endpoint.
- **Auth**: Test with real credentials/tokens in a sandbox environment if possible.
- **Payloads**: Validate that serialized data matches the recipient's expectations.

### CLI & Executables
- **Execution**: Run the actual compiled binary.
- **Side Effects**: Verify file creation, permission changes, and environment variable impacts.

## 4. Pre-Commit Checklist

| Category | Requirement | Status |
| :--- | :--- | :--- |
| **Unit** | Logic verified with mocks/stubs? | [ ] |
| **Unit** | Edge cases (null, empty, error) covered? | [ ] |
| **Integration** | Tested with real hardware/live API? | [ ] |
| **Integration** | All supported formats/parameters enumerated? | [ ] |
| **Integration** | Error paths (timeout, disconnect) verified? | [ ] |

## 5. Examples

### Good: Real-world Verification
> ``CpalCapture`` was tested using a MacBook Pro Microphone. Verified support for ``f32`` at 48kHz. Added explicit error handling for unsupported ``i16`` format discovered during testing.

### Bad: Mock-only Reliance
> ``CpalCapture`` logic passes all unit tests using a ``MockStream``.
> *Result: Fails in production because the real driver requires a specific buffer size not implemented in the mock.*

**WARNING**: If you cannot access the required hardware or API for integration testing, you MUST state this limitation clearly in the commit message and request manual verification from the user.
