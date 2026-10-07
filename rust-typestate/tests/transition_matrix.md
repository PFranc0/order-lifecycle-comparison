# Transition matrix

These examples exercise the public library API as an external consumer.
They are included in the crate documentation and run by `cargo test`.
The expected outcomes follow `docs/transitions.md`: 10 valid transitions
and 46 invalid state/event pairs. Each invalid pair has its own compile-fail
test, so one compiler error cannot hide another pair that unexpectedly compiles.

Valid cases check both the exact destination type and preservation of the ID.

## Created

### Submit → Submitted

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1);
let next: Order<Submitted> = order.submit();
assert_eq!(next.id(), 1);
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.reject();
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.request_cancel();
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1);
let _ = order.cancel_ack();
```

## Submitted

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let _ = order.submit();
```

### Ack → Accepted

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let next: Order<Accepted> = order.ack();
assert_eq!(next.id(), 1);
```

### Reject → Rejected

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let next: Order<Rejected> = order.reject();
assert_eq!(next.id(), 1);
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let _ = order.request_cancel();
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit();
let _ = order.cancel_ack();
```

## Accepted

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let _ = order.reject();
```

### PartialFill → PartiallyFilled

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let next: Order<PartiallyFilled> = order.partial_fill();
assert_eq!(next.id(), 1);
```

### FullFill → Filled

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let next: Order<Filled> = order.full_fill();
assert_eq!(next.id(), 1);
```

### RequestCancel → CancelPending

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let next: Order<CancelPending> = order.request_cancel();
assert_eq!(next.id(), 1);
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack();
let _ = order.cancel_ack();
```

## PartiallyFilled

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let _ = order.reject();
```

### PartialFill → PartiallyFilled

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let next: Order<PartiallyFilled> = order.partial_fill();
assert_eq!(next.id(), 1);
```

### FullFill → Filled

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let next: Order<Filled> = order.full_fill();
assert_eq!(next.id(), 1);
```

### RequestCancel → CancelPending

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let next: Order<CancelPending> = order.request_cancel();
assert_eq!(next.id(), 1);
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().partial_fill();
let _ = order.cancel_ack();
```

## Filled

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.reject();
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.request_cancel();
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().full_fill();
let _ = order.cancel_ack();
```

## CancelPending

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.reject();
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let _ = order.request_cancel();
```

### CancelAck → Cancelled

```rust
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel();
let next: Order<Cancelled> = order.cancel_ack();
assert_eq!(next.id(), 1);
```

## Cancelled

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.reject();
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.request_cancel();
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().ack().request_cancel().cancel_ack();
let _ = order.cancel_ack();
```

## Rejected

### Submit — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.submit();
```

### Ack — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.ack();
```

### Reject — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.reject();
```

### PartialFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.partial_fill();
```

### FullFill — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.full_fill();
```

### RequestCancel — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.request_cancel();
```

### CancelAck — invalid

```compile_fail
use rust_typestate::*;
let order = Order::<Created>::new(1).submit().reject();
let _ = order.cancel_ack();
```
