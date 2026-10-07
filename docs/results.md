# Results

The two implementations encode the same order lifecycle, but they differ in the
moment when invalid transitions are detected.

In the C++ FSM, every `OrderEvent` can be passed to `Order::apply`. This keeps
the API flexible, but invalid transitions compile and must be rejected at
runtime.

In the Rust Typestate implementation, invalid transitions are absent from the
public API for incompatible states. The API lives in a library, separating its
private fields and transition helper from consumers. The examples below are
covered by automated compile-fail tests using that public API.

## Automated Verification

Run the commands in [README.md](../README.md) from the repository root.

| Check | Coverage | Expected result |
|---|---|---|
| C++ state/event matrix | All 56 combinations | 10 accepted with the expected destination; 46 rejected with unchanged state |
| C++ flow examples | Four valid flows and five invalid-event examples | All checks pass in Debug and Release |
| Rust state/event matrix | 10 valid transitions | Each example compiles, runs, preserves the ID and returns the expected type |
| Rust state/event matrix | 46 invalid combinations | Each example independently fails to compile |
| Rust API boundary and ownership | Private helper, private fields and reuse after a move | Three independent compilation failures |
| Rust integration tests | Six valid flows | All pass, including repeated partial fills and cancellation after a partial fill |

The Rust matrix is in
[`tests/transition_matrix.md`](../rust-typestate/tests/transition_matrix.md),
included in the library documentation so `cargo test` executes it. There are
59 documentation tests in total, plus six integration tests. A compile-fail
test passes only when its example fails to compile.

The C++ matrix creates a fresh order for every state/event pair, reaches its
initial state through valid events and checks both the return value and final
state. Its checks are independent of `assert` and remain active in Release.
CTest reports failures through the executable's nonzero exit status.

## Invalid Rust Examples

`Order<Created>` does not expose `full_fill()`:

```rust
use rust_typestate::{Created, Order};
let order = Order::<Created>::new(1);
let order = order.full_fill();
```

`Order<Filled>` does not expose `request_cancel()`:

```rust
use rust_typestate::{Created, Order};
let order = Order::<Created>::new(2).submit().ack().full_fill();
let order = order.request_cancel();
```

`Order<Accepted>` does not expose `cancel_ack()`:

```rust
use rust_typestate::{Created, Order};
let order = Order::<Created>::new(3).submit().ack();
let order = order.cancel_ack();
```

`Order<Accepted>` does not expose `reject()`:

```rust
use rust_typestate::{Created, Order};
let order = Order::<Created>::new(4).submit().ack();
let order = order.reject();
```

`Order<Rejected>` does not expose `submit()`:

```rust
use rust_typestate::{Created, Order};
let order = Order::<Created>::new(5).submit().reject();
let order = order.submit();
```

## Interpretation

These examples show that Typestate can move part of the validation burden from
runtime to compile time. It does not eliminate every runtime validation needed in
a real trading system. Data-dependent rules, external messages, duplicate
events, and integration failures still require dynamic checks.
