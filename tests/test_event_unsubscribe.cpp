#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "imguix/core/pubsub/Event.hpp"
#include "imguix/core/pubsub/EventListener.hpp"

#define private public
#include "imguix/core/pubsub/EventBus.hpp"
#undef private

#include "imguix/core/pubsub/EventAwaiter.hpp"
#include "imguix/core/pubsub/EventMediator.hpp"

using namespace ImGuiX::Pubsub;

struct EvA : Event {
    std::type_index type() const override { return typeid(EvA); }
    const char* name() const override { return "EvA"; }
    std::unique_ptr<Event> clone() const override { return std::make_unique<EvA>(*this); }
};

struct EvB : Event {
    std::type_index type() const override { return typeid(EvB); }
    const char* name() const override { return "EvB"; }
    std::unique_ptr<Event> clone() const override { return std::make_unique<EvB>(*this); }
};

int main() {
    EventBus bus;
    EventMediator med(bus);

    med.subscribe<EvA>([](const EvA&) {});
    if (bus.m_event_callbacks.size() != 1) {
        std::cerr << "callback map not updated\n";
        return 1;
    }
    med.unsubscribe<EvA>();
    if (!bus.m_event_callbacks.empty()) {
        std::cerr << "callback map not cleared\n";
        return 1;
    }

    med.subscribe<EvA>();
    if (bus.m_event_listeners.size() != 1) {
        std::cerr << "listener map not updated\n";
        return 1;
    }
    med.unsubscribe<EvA>();
    if (!bus.m_event_listeners.empty()) {
        std::cerr << "listener map not cleared\n";
        return 1;
    }

    med.subscribe<EvA>([](const EvA&) {});
    med.subscribe<EvB>();
    if (bus.m_event_callbacks.size() != 1 || bus.m_event_listeners.size() != 1) {
        std::cerr << "maps not updated for multiple events\n";
        return 1;
    }
    med.unsubscribeAll();
    if (!bus.m_event_callbacks.empty() || !bus.m_event_listeners.empty()) {
        std::cerr << "maps not cleared after unsubscribeAll\n";
        return 1;
    }

    std::cout << "EventBus unsubscribe tests passed\n";
    return 0;
}
