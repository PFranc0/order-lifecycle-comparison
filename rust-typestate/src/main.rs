use std::marker::PhantomData;

pub struct Created;
pub struct Submitted;
pub struct Accepted;
pub struct PartiallyFilled;
pub struct Filled;
pub struct CancelPending;
pub struct Cancelled;
pub struct Rejected;

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

fn demonstrate_valid_flows() {
    let filled = Order::<Created>::new(1).submit().ack().full_fill();
    assert_eq!(filled.id(), 1);

    let rejected = Order::<Created>::new(2).submit().reject();
    assert_eq!(rejected.id(), 2);

    let cancelled = Order::<Created>::new(3)
        .submit()
        .ack()
        .request_cancel()
        .cancel_ack();
    assert_eq!(cancelled.id(), 3);

    let filled_after_partial = Order::<Created>::new(4)
        .submit()
        .ack()
        .partial_fill()
        .full_fill();
    assert_eq!(filled_after_partial.id(), 4);
}

fn main() {
    demonstrate_valid_flows();

    // These calls are intentionally invalid and should not compile:
    //
    // let order = Order::<Created>::new(10);
    // let order = order.full_fill();
    //
    // let order = Order::<Created>::new(11).submit().ack().full_fill();
    // let order = order.request_cancel();
    //
    // let order = Order::<Created>::new(12).submit().ack();
    // let order = order.cancel_ack();
    //
    // let order = Order::<Created>::new(13).submit().ack();
    // let order = order.reject();
    //
    // let order = Order::<Created>::new(14).submit().reject();
    // let order = order.submit();

    println!("Rust Typestate: all valid flows compiled and ran.");
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn created_to_filled() {
        let order = Order::<Created>::new(1).submit().ack().full_fill();

        assert_eq!(order.id(), 1);
    }

    #[test]
    fn created_to_rejected() {
        let order = Order::<Created>::new(2).submit().reject();

        assert_eq!(order.id(), 2);
    }

    #[test]
    fn created_to_cancelled() {
        let order = Order::<Created>::new(3)
            .submit()
            .ack()
            .request_cancel()
            .cancel_ack();

        assert_eq!(order.id(), 3);
    }

    #[test]
    fn created_to_partially_filled_to_filled() {
        let order = Order::<Created>::new(4)
            .submit()
            .ack()
            .partial_fill()
            .full_fill();

        assert_eq!(order.id(), 4);
    }

    #[test]
    fn multiple_partial_fills_then_filled() {
        let order = Order::<Created>::new(5)
            .submit()
            .ack()
            .partial_fill()
            .partial_fill()
            .full_fill();

        assert_eq!(order.id(), 5);
    }
}
