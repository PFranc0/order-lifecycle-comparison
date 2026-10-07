#![doc = include_str!("../tests/transition_matrix.md")]

use std::marker::PhantomData;

pub struct Created;
pub struct Submitted;
pub struct Accepted;
pub struct PartiallyFilled;
pub struct Filled;
pub struct CancelPending;
pub struct Cancelled;
pub struct Rejected;

/// An order whose state determines the available transitions.
///
/// Consumers cannot call the internal transition helper:
///
/// ```compile_fail
/// use rust_typestate::{Created, Filled, Order};
/// let _ = Order::<Created>::new(1).transition::<Filled>();
/// ```
///
/// Consumers cannot construct an arbitrary state through the private fields:
///
/// ```compile_fail
/// use std::marker::PhantomData;
/// use rust_typestate::{Filled, Order};
/// let _: Order<Filled> = Order { id: 1, _state: PhantomData };
/// ```
///
/// A transition consumes the previous value, preventing reuse:
///
/// ```compile_fail
/// use rust_typestate::{Created, Order};
/// let order = Order::<Created>::new(1);
/// let _submitted = order.submit();
/// let _reused = order.submit();
/// ```
pub struct Order<State> {
    id: u64,
    _state: PhantomData<State>,
}

impl<State> Order<State> {
    pub fn id(&self) -> u64 {
        self.id
    }

    fn transition<Next>(self) -> Order<Next> {
        Order {
            id: self.id,
            _state: PhantomData,
        }
    }
}

impl Order<Created> {
    pub fn new(id: u64) -> Self {
        Self {
            id,
            _state: PhantomData,
        }
    }

    pub fn submit(self) -> Order<Submitted> {
        self.transition()
    }
}

impl Order<Submitted> {
    pub fn ack(self) -> Order<Accepted> {
        self.transition()
    }

    pub fn reject(self) -> Order<Rejected> {
        self.transition()
    }
}

impl Order<Accepted> {
    pub fn partial_fill(self) -> Order<PartiallyFilled> {
        self.transition()
    }

    pub fn full_fill(self) -> Order<Filled> {
        self.transition()
    }

    pub fn request_cancel(self) -> Order<CancelPending> {
        self.transition()
    }
}

impl Order<PartiallyFilled> {
    pub fn partial_fill(self) -> Order<PartiallyFilled> {
        self.transition()
    }

    pub fn full_fill(self) -> Order<Filled> {
        self.transition()
    }

    pub fn request_cancel(self) -> Order<CancelPending> {
        self.transition()
    }
}

impl Order<CancelPending> {
    pub fn cancel_ack(self) -> Order<Cancelled> {
        self.transition()
    }
}
