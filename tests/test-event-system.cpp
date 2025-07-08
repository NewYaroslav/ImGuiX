#include <iostream>
#include <memory>
#include <string>

#include "imguix/utils/pubsub.hpp"

using namespace ImGuiX::utils;

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

// Пример слушателя, реализующего интерфейс
class MyListener : public EventMediator {
public:
    bool received = false;

    explicit MyListener(EventHub& hub) : EventMediator(&hub) {}
    
    virtual ~MyListener() = default;

    void on_event(const Event* const event) override {
        if (auto* e = dynamic_cast<const MyEvent*>(event)) {
            std::cout << "MyListener received: " << e->message << "\n";
            received = true;
        }
    }
};

// Главный тест
int main() {
    EventHub hub;
    MyListener listener(hub);

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
    listener.notify_async(std::move(async_event));
    hub.process();

    std::cout << "Test passed\n";
    std::cin.get();
    return 0;
}