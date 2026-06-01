#include <cassert>
#include <iostream>
#include <string_view>

enum class OrderState {
    Created,
    Submitted,
    Accepted,
    PartiallyFilled,
    Filled,
    CancelPending,
    Cancelled,
    Rejected
};

enum class OrderEvent {
    Submit,
    Ack,
    Reject,
    PartialFill,
    FullFill,
    RequestCancel,
    CancelAck
};

std::string_view to_string(OrderState state) {
    switch (state) {
        case OrderState::Created:
            return "Created";
        case OrderState::Submitted:
            return "Submitted";
        case OrderState::Accepted:
            return "Accepted";
        case OrderState::PartiallyFilled:
            return "PartiallyFilled";
        case OrderState::Filled:
            return "Filled";
        case OrderState::CancelPending:
            return "CancelPending";
        case OrderState::Cancelled:
            return "Cancelled";
        case OrderState::Rejected:
            return "Rejected";
    }

    return "Unknown";
}

std::string_view to_string(OrderEvent event) {
    switch (event) {
        case OrderEvent::Submit:
            return "Submit";
        case OrderEvent::Ack:
            return "Ack";
        case OrderEvent::Reject:
            return "Reject";
        case OrderEvent::PartialFill:
            return "PartialFill";
        case OrderEvent::FullFill:
            return "FullFill";
        case OrderEvent::RequestCancel:
            return "RequestCancel";
        case OrderEvent::CancelAck:
            return "CancelAck";
    }

    return "Unknown";
}

class Order {
public:
    bool apply(OrderEvent event) {
        switch (state_) {
            case OrderState::Created:
                return transition_if(event, OrderEvent::Submit, OrderState::Submitted);

            case OrderState::Submitted:
                if (event == OrderEvent::Ack) {
                    state_ = OrderState::Accepted;
                    return true;
                }
                if (event == OrderEvent::Reject) {
                    state_ = OrderState::Rejected;
                    return true;
                }
                return false;

            case OrderState::Accepted:
                if (event == OrderEvent::PartialFill) {
                    state_ = OrderState::PartiallyFilled;
                    return true;
                }
                if (event == OrderEvent::FullFill) {
                    state_ = OrderState::Filled;
                    return true;
                }
                if (event == OrderEvent::RequestCancel) {
                    state_ = OrderState::CancelPending;
                    return true;
                }
                return false;

            case OrderState::PartiallyFilled:
                if (event == OrderEvent::PartialFill) {
                    state_ = OrderState::PartiallyFilled;
                    return true;
                }
                if (event == OrderEvent::FullFill) {
                    state_ = OrderState::Filled;
                    return true;
                }
                if (event == OrderEvent::RequestCancel) {
                    state_ = OrderState::CancelPending;
                    return true;
                }
                return false;

            case OrderState::CancelPending:
                return transition_if(event, OrderEvent::CancelAck, OrderState::Cancelled);

            case OrderState::Filled:
            case OrderState::Cancelled:
            case OrderState::Rejected:
                return false;
        }

        return false;
    }

    OrderState state() const {
        return state_;
    }

private:
    bool transition_if(OrderEvent event, OrderEvent expected, OrderState next_state) {
        if (event != expected) {
            return false;
        }

        state_ = next_state;
        return true;
    }

    OrderState state_ = OrderState::Created;
};

void test_created_to_filled() {
    Order order;

    assert(order.state() == OrderState::Created);
    assert(order.apply(OrderEvent::Submit));
    assert(order.state() == OrderState::Submitted);
    assert(order.apply(OrderEvent::Ack));
    assert(order.state() == OrderState::Accepted);
    assert(order.apply(OrderEvent::FullFill));
    assert(order.state() == OrderState::Filled);
}

void test_created_to_rejected() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Reject));
    assert(order.state() == OrderState::Rejected);
}

void test_created_to_cancelled() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Ack));
    assert(order.apply(OrderEvent::RequestCancel));
    assert(order.state() == OrderState::CancelPending);
    assert(order.apply(OrderEvent::CancelAck));
    assert(order.state() == OrderState::Cancelled);
}

void test_created_to_partially_filled_to_filled() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Ack));
    assert(order.apply(OrderEvent::PartialFill));
    assert(order.state() == OrderState::PartiallyFilled);
    assert(order.apply(OrderEvent::FullFill));
    assert(order.state() == OrderState::Filled);
}

void test_invalid_full_fill_on_created() {
    Order order;

    assert(!order.apply(OrderEvent::FullFill));
    assert(order.state() == OrderState::Created);
}

void test_invalid_request_cancel_on_filled() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Ack));
    assert(order.apply(OrderEvent::FullFill));
    assert(order.state() == OrderState::Filled);
    assert(!order.apply(OrderEvent::RequestCancel));
    assert(order.state() == OrderState::Filled);
}

void test_invalid_cancel_ack_on_accepted() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Ack));
    assert(order.state() == OrderState::Accepted);
    assert(!order.apply(OrderEvent::CancelAck));
    assert(order.state() == OrderState::Accepted);
}

void test_invalid_reject_on_accepted() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Ack));
    assert(order.state() == OrderState::Accepted);
    assert(!order.apply(OrderEvent::Reject));
    assert(order.state() == OrderState::Accepted);
}

void test_invalid_submit_on_rejected() {
    Order order;

    assert(order.apply(OrderEvent::Submit));
    assert(order.apply(OrderEvent::Reject));
    assert(order.state() == OrderState::Rejected);
    assert(!order.apply(OrderEvent::Submit));
    assert(order.state() == OrderState::Rejected);
}

int main() {
    test_created_to_filled();
    test_created_to_rejected();
    test_created_to_cancelled();
    test_created_to_partially_filled_to_filled();

    test_invalid_full_fill_on_created();
    test_invalid_request_cancel_on_filled();
    test_invalid_cancel_ack_on_accepted();
    test_invalid_reject_on_accepted();
    test_invalid_submit_on_rejected();

    std::cout << "C++ FSM: all runtime validation tests passed.\n";
    return 0;
}
