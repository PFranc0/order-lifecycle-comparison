use rust_typestate::{Created, Order};

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

#[test]
fn partially_filled_to_cancelled() {
    let order = Order::<Created>::new(6)
        .submit()
        .ack()
        .partial_fill()
        .request_cancel()
        .cancel_ack();

    assert_eq!(order.id(), 6);
}
