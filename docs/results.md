# Results

The two implementations encode the same order lifecycle, but they differ in the
moment when invalid transitions are detected.

In the C++ FSM, every `OrderEvent` can be passed to `Order::apply`. This keeps
the API flexible, but invalid transitions compile and must be rejected at
runtime.

In the Rust Typestate implementation, invalid transitions are absent from the
API for incompatible states. These examples should fail at compile time if
uncommented in `rust-typestate/src/main.rs`.

## Invalid Rust Examples

`Order<Created>` does not expose `full_fill()`:

```rust
let order = Order::<Created>::new(1);
let order = order.full_fill();
```

`Order<Filled>` does not expose `request_cancel()`:

```rust
let order = Order::<Created>::new(2).submit().ack().full_fill();
let order = order.request_cancel();
```

`Order<Accepted>` does not expose `cancel_ack()`:

```rust
let order = Order::<Created>::new(3).submit().ack();
let order = order.cancel_ack();
```

`Order<Accepted>` does not expose `reject()`:

```rust
let order = Order::<Created>::new(4).submit().ack();
let order = order.reject();
```

`Order<Rejected>` does not expose `submit()`:

```rust
let order = Order::<Created>::new(5).submit().reject();
let order = order.submit();
```

## Interpretation

These examples show that Typestate can move part of the validation burden from
runtime to compile time. It does not eliminate every runtime validation needed in
a real trading system. Data-dependent rules, external messages, duplicate
events, and integration failures still require dynamic checks.
