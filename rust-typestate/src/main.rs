use rust_typestate::{Created, Order};

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

    println!("Rust Typestate: all valid flows compiled and ran.");
}
