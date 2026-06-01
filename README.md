# Order Lifecycle Comparison

This repository is a proof of concept for an academic article about dynamic
validation versus static validation in a simplified financial order lifecycle.

It contains two implementations of the same state model:

- `cpp-fsm`: a conventional finite state machine in C++ with runtime validation.
- `rust-typestate`: a Rust Typestate model that restricts valid transitions at
  compile time.

The goal is not to benchmark performance, model real HFT infrastructure, or
claim that one programming language is generally superior to another. The goal
is to make the validation boundary visible: in the C++ FSM, invalid transitions
can be written and compiled, so they must be rejected at runtime; in the Rust
Typestate version, some invalid transitions are not expressible through the API.

## State Model

The simplified order lifecycle has eight states:

- `Created`
- `Submitted`
- `Accepted`
- `PartiallyFilled`
- `Filled`
- `CancelPending`
- `Cancelled`
- `Rejected`

It has seven events:

- `Submit`
- `Ack`
- `Reject`
- `PartialFill`
- `FullFill`
- `RequestCancel`
- `CancelAck`

The full transition table is documented in
[docs/transitions.md](docs/transitions.md).

## Runtime Validation Versus Compile-Time Validation

In the C++ implementation, an `Order` stores its current `OrderState`
internally. The method `apply(OrderEvent event)` accepts any event and checks at
runtime whether the current state/event pair is valid. Invalid transitions return
`false` and leave the order state unchanged.

In the Rust implementation, each state is represented by a distinct type. An
`Order<Created>` exposes `submit()`, but does not expose `full_fill()`. An
`Order<Filled>` does not expose `request_cancel()`. These calls fail before the
program can run, because the methods do not exist for those state types.

| Invalid transition | C++ FSM | Rust Typestate |
|---|---|---|
| FullFill on Created | Compiles; returns false at runtime | Does not compile |
| RequestCancel on Filled | Compiles; returns false at runtime | Does not compile |
| CancelAck on Accepted | Compiles; returns false at runtime | Does not compile |
| Reject on Accepted | Compiles; returns false at runtime | Does not compile |
| Submit on Rejected | Compiles; returns false at runtime | Does not compile |

See [docs/results.md](docs/results.md) for examples of invalid Rust calls.

## Running The C++ FSM

```sh
cmake -S cpp-fsm -B cpp-fsm/build
cmake --build cpp-fsm/build
./cpp-fsm/build/cpp_fsm
```

Expected output:

```text
C++ FSM: all runtime validation tests passed.
```

## Running The Rust Typestate Model

```sh
cd rust-typestate
cargo test
cargo run
```

Expected runtime output:

```text
Rust Typestate: all valid flows compiled and ran.
```
