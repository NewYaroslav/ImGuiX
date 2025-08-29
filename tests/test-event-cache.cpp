#include <iostream>
#include <optional>

#include "imguix/core/pubsub.hpp"

using namespace ImGuiX::Pubsub;

struct ValueEvent : Event {
    int value{};
    explicit ValueEvent(int v) : value(v) {}
    std::type_index type() const override { return typeid(ValueEvent); }
    const char* name() const override { return "ValueEvent"; }
    std::unique_ptr<Event> clone() const override {return std::make_unique<ValueEvent>(*this);}
};

int main() {
    EventBus bus;
    EventMediator med(bus);

    med.registerCachedEvent<ValueEvent>("A");
    med.registerCachedEvent<ValueEvent>("B", [](const ValueEvent& e) { return e.value > 5; });

    ValueEvent e1(1);
    med.notify(e1);
    auto a = med.getCachedEvent<ValueEvent>("A");
    auto b = med.getCachedEvent<ValueEvent>("B");
    if (!a || a->value != 1) { std::cerr << "A missing first event\n"; return 1; }
    if (b) { std::cerr << "B should be empty\n"; return 1; }

    ValueEvent e2(7);
    med.notify(e2);
    a = med.getCachedEvent<ValueEvent>("A");
    b = med.getCachedEvent<ValueEvent>("B");
    if (!a || a->value != 7) { std::cerr << "A missing second event\n"; return 1; }
    if (!b || b->value != 7) { std::cerr << "B missing second event\n"; return 1; }

    med.unregisterCachedEvent("B");
    ValueEvent e3(9);
    med.notify(e3);
    a = med.getCachedEvent<ValueEvent>("A");
    b = med.getCachedEvent<ValueEvent>("B");
    if (!a || a->value != 9) { std::cerr << "A missing third event\n"; return 1; }
    if (b) { std::cerr << "B should be removed\n"; return 1; }

    med.registerCachedEvent<ValueEvent>("A");
    ValueEvent e4(11);
    med.notify(e4);
    a = med.getCachedEvent<ValueEvent>("A");
    if (!a || a->value != 11) { std::cerr << "A not updated after re-register\n"; return 1; }

    std::cout << "All cached event tests passed\n";
    return 0;
}
