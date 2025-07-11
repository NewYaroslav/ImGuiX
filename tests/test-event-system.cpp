#include <iostream>
#include <memory>
#include <string>

#include "imguix/core/pubsub.hpp"

using namespace ImGuiX::Pubsub;

// Пример конкретного события
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

// Главный тест
int main() {
    EventBus bus;
    MyListener listener(bus);

    // Подписка на MyEvent
    listener.subscribe<MyEvent>();

    // Подписка на MyEvent через callback
    bool callback_received = false;
    listener.subscribe<MyEvent>([&callback_received](const MyEvent& e) {
        std::cout << "Callback received: " << e.message << "\n";
        callback_received = true;
    });

    // Уведомление напрямую
    MyEvent event("Hello world!");
    listener.notify(event);

    if (!listener.received || !callback_received) {
        std::cerr << "Test failed: not all listeners received event\n";
        std::cin.get();
        return 1;
    }

    // Проверка асинхронной очереди
    auto async_event = std::make_unique<MyEvent>("From async queue");
    listener.notifyAsync(std::move(async_event));
    bus.process();

    std::cout << "Test passed\n";
    std::cin.get();
    return 0;
}