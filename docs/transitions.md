# Transition Model

This document defines the simplified financial order lifecycle used by both
implementations.

## States

| State | Description |
|---|---|
| `Created` | Order created locally, not yet submitted. |
| `Submitted` | Order submitted to the trading system. |
| `Accepted` | Order accepted and available for execution or cancellation. |
| `PartiallyFilled` | Order partially executed, with remaining quantity. |
| `Filled` | Order fully executed. |
| `CancelPending` | Cancellation requested, waiting for acknowledgement. |
| `Cancelled` | Order cancelled. |
| `Rejected` | Order rejected by the trading system. |

## Events

| Event | Meaning |
|---|---|
| `Submit` | Submit the local order. |
| `Ack` | Acknowledge that the submitted order was accepted. |
| `Reject` | Reject the submitted order. |
| `PartialFill` | Report a partial execution. |
| `FullFill` | Report a full execution. |
| `RequestCancel` | Request cancellation. |
| `CancelAck` | Acknowledge cancellation. |

## Valid Transitions

| Initial state | Event | Final state |
|---|---|---|
| `Created` | `Submit` | `Submitted` |
| `Submitted` | `Ack` | `Accepted` |
| `Submitted` | `Reject` | `Rejected` |
| `Accepted` | `PartialFill` | `PartiallyFilled` |
| `Accepted` | `FullFill` | `Filled` |
| `Accepted` | `RequestCancel` | `CancelPending` |
| `PartiallyFilled` | `PartialFill` | `PartiallyFilled` |
| `PartiallyFilled` | `FullFill` | `Filled` |
| `PartiallyFilled` | `RequestCancel` | `CancelPending` |
| `CancelPending` | `CancelAck` | `Cancelled` |

## Combination Count

```text
states = 8
events = 7
total combinations = 8 * 7 = 56
valid transitions = 10
invalid combinations = 56 - 10 = 46
```

The proof of concept compares how the C++ FSM and the Rust Typestate model
handle these 46 invalid combinations.

All 56 pairs are checked automatically: the C++ executable tests the runtime
result and final state, while the Rust documentation tests check valid return
types and compilation failures for invalid calls. See
[results.md](results.md) for coverage and [README.md](../README.md) for commands.
