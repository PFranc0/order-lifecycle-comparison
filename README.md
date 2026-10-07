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

The Rust API lives in `src/lib.rs`, while the example program and integration
tests are separate consumers. The internal transition helper and state fields
are private, so consumers cannot use them to bypass the permitted transitions.

| Invalid transition | C++ FSM | Rust Typestate |
|---|---|---|
| FullFill on Created | Compiles; returns false at runtime | Does not compile |
| RequestCancel on Filled | Compiles; returns false at runtime | Does not compile |
| CancelAck on Accepted | Compiles; returns false at runtime | Does not compile |
| Reject on Accepted | Compiles; returns false at runtime | Does not compile |
| Submit on Rejected | Compiles; returns false at runtime | Does not compile |

See [docs/results.md](docs/results.md) for examples and automated test coverage.

## Development Environment

Open the repository in its devcontainer to use C++, CMake and Rust together.
The Dockerfile supplies the C++ environment, and `devcontainer.json` adds Rust
through the Rust feature. No third-party libraries are required by either model.

## Running The C++ FSM

```sh
cmake -S cpp-fsm -B cpp-fsm/build -DCMAKE_BUILD_TYPE=Debug
cmake --build cpp-fsm/build
ctest --test-dir cpp-fsm/build --output-on-failure
./cpp-fsm/build/cpp_fsm
```

Expected output:

```text
C++ FSM: 56 combinations checked (10 valid, 46 invalid).
C++ FSM: all runtime validation tests passed.
```

To verify the same checks in Release:

```sh
cmake -S cpp-fsm -B cpp-fsm/build/release -DCMAKE_BUILD_TYPE=Release
cmake --build cpp-fsm/build/release
ctest --test-dir cpp-fsm/build/release --output-on-failure
```

Checks remain active with `NDEBUG`; a failed check produces a nonzero exit code.

## Running The Rust Typestate Model

```sh
cargo test --manifest-path rust-typestate/Cargo.toml
cargo test --manifest-path rust-typestate/Cargo.toml --release
cargo run --manifest-path rust-typestate/Cargo.toml
```

Expected runtime output:

```text
Rust Typestate: all valid flows compiled and ran.
```

`cargo test` runs six integration tests and 59 documentation tests: the 56
state/event combinations and three checks for private API access and ownership.
Each of the 46 invalid combinations is a separate compile-fail test. The ten
valid combinations check the destination type and order ID.

## Reproducing the Article

The source repository is
[PFranc0/order-lifecycle-comparison](https://github.com/PFranc0/order-lifecycle-comparison).
When citing the experiment, record the tested commit and tool versions:

```sh
git rev-parse HEAD
c++ --version
cmake --version
rustc --version
cargo --version
```

Run these commands in the same environment used for the tests. The devcontainer
does not pin every compiler version, so its configuration alone does not identify
the exact experimental environment.
