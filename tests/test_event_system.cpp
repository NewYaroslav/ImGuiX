#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "imguix/core/pubsub.hpp"

using namespace ImGuiX::Pubsub;

// Example of a concrete event
class MyEvent : public Event {
public:
    std::string message;

    MyEvent(std::string msg) : message(std::move(msg)) {}

    std::type_index type() const override {
        return typeid(MyEvent);
    }

    const char* name() const override {
        return "MyEvent";
    }
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<MyEvent>(*this);
    }
};

// Example listener that reacts to MyEvent and demonstrates different cast methods.
class MyListener : public EventMediator {
public:
    bool received = false;

    explicit MyListener(EventBus& bus) : EventMediator(&bus) {}

    virtual ~MyListener() = default;

    void onEvent(const Event* const event) override {
        // Check using is<T>() before attempting access
        if (event->is<MyEvent>()) {
            // Version 1: Throws on type mismatch (safe if checked beforehand)
            std::cout << "MyListener received:\n";
            std::cout << "(V1): " << event->asRef<MyEvent>().message << "\n";

            // Version 2: Returns nullptr on type mismatch (safe but needs null-check if used directly)
            std::cout << "(V2): " << event->as<MyEvent>()->message << "\n";

            received = true;
        }

        // Version 3: Traditional dynamic_cast (returns nullptr if mismatch)
        if (const auto* e = dynamic_cast<const MyEvent*>(event)) {
            std::cout << "(V3): " << e->message << "\n";
        }
    }
};

// Main test
int main() {
    EventBus bus;
    MyListener listener(bus);

    // Subscribe to MyEvent
    listener.subscribe<MyEvent>();

    // Subscribe to MyEvent via callback
    bool callback_received = false;
    listener.subscribe<MyEvent>([&callback_received](const MyEvent& e) {
        std::cout << "Callback received: " << e.message << "\n";
        callback_received = true;
    });

    // Direct notification
    MyEvent event("Hello world!");
    listener.notify(event);

    bool success = listener.received && callback_received;
    assert(success && "Test failed: not all listeners received event");

    // Test asynchronous queue
    listener.received = false;
    auto async_event = std::make_unique<MyEvent>("From async queue");
    listener.notifyAsync(std::move(async_event));
    bus.process();
    assert(listener.received && "Async event not processed");

    std::cout << "Test passed\n";
    return success ? 0 : 1;
}
