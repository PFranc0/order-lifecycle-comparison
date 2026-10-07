#include <array>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

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

// Unlike assert(), this check evaluates its argument even when NDEBUG is set.
void check(bool condition, std::string_view expression, int line) {
    if (!condition) {
        throw std::runtime_error("line " + std::to_string(line) + ": " +
                                 std::string(expression));
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

void test_created_to_filled() {
    Order order;

    CHECK(order.state() == OrderState::Created);
    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.state() == OrderState::Submitted);
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.state() == OrderState::Accepted);
    CHECK(order.apply(OrderEvent::FullFill));
    CHECK(order.state() == OrderState::Filled);
}

void test_created_to_rejected() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Reject));
    CHECK(order.state() == OrderState::Rejected);
}

void test_created_to_cancelled() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.apply(OrderEvent::RequestCancel));
    CHECK(order.state() == OrderState::CancelPending);
    CHECK(order.apply(OrderEvent::CancelAck));
    CHECK(order.state() == OrderState::Cancelled);
}

void test_created_to_partially_filled_to_filled() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.apply(OrderEvent::PartialFill));
    CHECK(order.state() == OrderState::PartiallyFilled);
    CHECK(order.apply(OrderEvent::FullFill));
    CHECK(order.state() == OrderState::Filled);
}

void test_invalid_full_fill_on_created() {
    Order order;

    CHECK(!order.apply(OrderEvent::FullFill));
    CHECK(order.state() == OrderState::Created);
}

void test_invalid_request_cancel_on_filled() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.apply(OrderEvent::FullFill));
    CHECK(order.state() == OrderState::Filled);
    CHECK(!order.apply(OrderEvent::RequestCancel));
    CHECK(order.state() == OrderState::Filled);
}

void test_invalid_cancel_ack_on_accepted() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.state() == OrderState::Accepted);
    CHECK(!order.apply(OrderEvent::CancelAck));
    CHECK(order.state() == OrderState::Accepted);
}

void test_invalid_reject_on_accepted() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Ack));
    CHECK(order.state() == OrderState::Accepted);
    CHECK(!order.apply(OrderEvent::Reject));
    CHECK(order.state() == OrderState::Accepted);
}

void test_invalid_submit_on_rejected() {
    Order order;

    CHECK(order.apply(OrderEvent::Submit));
    CHECK(order.apply(OrderEvent::Reject));
    CHECK(order.state() == OrderState::Rejected);
    CHECK(!order.apply(OrderEvent::Submit));
    CHECK(order.state() == OrderState::Rejected);
}

void test_transition_matrix() {
    using S = OrderState;
    using E = OrderEvent;

    struct StateCase {
        S state;
        std::vector<E> path;
    };
    const std::array<StateCase, 8> states{{
        {S::Created, {}},
        {S::Submitted, {E::Submit}},
        {S::Accepted, {E::Submit, E::Ack}},
        {S::PartiallyFilled, {E::Submit, E::Ack, E::PartialFill}},
        {S::Filled, {E::Submit, E::Ack, E::FullFill}},
        {S::CancelPending, {E::Submit, E::Ack, E::RequestCancel}},
        {S::Cancelled, {E::Submit, E::Ack, E::RequestCancel, E::CancelAck}},
        {S::Rejected, {E::Submit, E::Reject}},
    }};
    const std::array<E, 7> events{{
        E::Submit, E::Ack, E::Reject, E::PartialFill,
        E::FullFill, E::RequestCancel, E::CancelAck,
    }};

    struct Transition {
        S initial;
        E event;
        S final;
    };
    // Expected outcomes from the model in docs/transitions.md.
    const std::array<Transition, 10> valid{{
        {S::Created, E::Submit, S::Submitted},
        {S::Submitted, E::Ack, S::Accepted},
        {S::Submitted, E::Reject, S::Rejected},
        {S::Accepted, E::PartialFill, S::PartiallyFilled},
        {S::Accepted, E::FullFill, S::Filled},
        {S::Accepted, E::RequestCancel, S::CancelPending},
        {S::PartiallyFilled, E::PartialFill, S::PartiallyFilled},
        {S::PartiallyFilled, E::FullFill, S::Filled},
        {S::PartiallyFilled, E::RequestCancel, S::CancelPending},
        {S::CancelPending, E::CancelAck, S::Cancelled},
    }};

    int accepted = 0;
    int rejected = 0;
    for (const auto& state_case : states) {
        for (const auto event : events) {
            Order order;
            for (const auto setup_event : state_case.path) {
                CHECK(order.apply(setup_event));
            }
            CHECK(order.state() == state_case.state);

            bool expected_success = false;
            auto expected_state = state_case.state;
            for (const auto& transition : valid) {
                if (transition.initial == state_case.state && transition.event == event) {
                    expected_success = true;
                    expected_state = transition.final;
                    break;
                }
            }

            const bool success = order.apply(event);
            if (success != expected_success || order.state() != expected_state) {
                throw std::runtime_error(
                    "transition mismatch: " + std::string(to_string(state_case.state)) +
                    " / " + std::string(to_string(event)));
            }
            if (success) {
                ++accepted;
            } else {
                ++rejected;
            }
        }
    }
    CHECK(accepted == 10);
    CHECK(rejected == 46);
    std::cout << "C++ FSM: 56 combinations checked (10 valid, 46 invalid).\n";
}

int main() {
    try {
        test_created_to_filled();
        test_created_to_rejected();
        test_created_to_cancelled();
        test_created_to_partially_filled_to_filled();

        test_invalid_full_fill_on_created();
        test_invalid_request_cancel_on_filled();
        test_invalid_cancel_ack_on_accepted();
        test_invalid_reject_on_accepted();
        test_invalid_submit_on_rejected();
        test_transition_matrix();

        std::cout << "C++ FSM: all runtime validation tests passed.\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "C++ FSM: validation failed: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
