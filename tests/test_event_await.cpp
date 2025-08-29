#include <iostream>
#include <thread>
#include <chrono>

#include "imguix/core/pubsub.hpp"

using namespace ImGuiX::Pubsub;
using namespace std::chrono_literals;

struct TestEvent : Event {
    std::type_index type() const override { return typeid(TestEvent); }
    const char* name() const override { return "TestEvent"; }
    std::unique_ptr<Event> clone() const override {return std::make_unique<TestEvent>(*this);}
};

int main() {
    EventBus bus;
    EventMediator med(bus);

    // 1. Timeout fires + on_timeout
    bool called = false; int timeout_calls = 0;
    auto aw1 = med.awaitEach<TestEvent>([](const TestEvent&){ return true; },
        [&](const TestEvent&){ called = true; }, 5ms, [&]{ ++timeout_calls; });
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(2ms);
        bus.process();
    }
    if (called || timeout_calls != 1 || aw1->isActive()) { std::cerr << "Timeout test failed\n"; return 1; }

    // 2. External cancel (no timeout callback)
    called = false; timeout_calls = 0;
    CancellationSource src2; AwaitOptions opt2; opt2.token = src2.token();
    opt2.on_timeout = [&]{ ++timeout_calls; };
    auto aw2 = med.awaitEach<TestEvent>([](const TestEvent&){ return true; },
        [&](const TestEvent&){ called = true; }, opt2);
    src2.cancel();
    bus.process();
    if (called || timeout_calls != 0 || aw2->isActive()) { std::cerr << "External cancel failed\n"; return 1; }

    // 3. Race cancel vs event (event queued but cancelled before process)
    called = false;
    CancellationSource src3; AwaitOptions opt3; opt3.token = src3.token();
    auto aw3 = med.awaitEach<TestEvent>([](const TestEvent&){ return true; },
        [&](const TestEvent&){ called = true; }, opt3);
    bus.notifyAsync(std::make_unique<TestEvent>());
    src3.cancel();
    bus.process();
    if (called || aw3->isActive()) { std::cerr << "Race cancel vs event failed\n"; return 1; }

    // 4. Backward compatibility
    called = false;
    med.awaitOnce<TestEvent>([&](const TestEvent&){ called = true; });
    bus.notifyAsync(std::make_unique<TestEvent>());
    bus.process();
    if (!called) { std::cerr << "Backward compatibility failed\n"; return 1; }

    // 5. Multi awaiters + one token
    called = false; bool calledB = false;
    CancellationSource src5; AwaitOptions opt5; opt5.token = src5.token();
    auto aw5a = med.awaitEach<TestEvent>([](const TestEvent&){ return true; },
        [&](const TestEvent&){ called = true; }, opt5);
    auto aw5b = med.awaitEach<TestEvent>([](const TestEvent&){ return true; },
        [&](const TestEvent&){ calledB = true; }, opt5);
    src5.cancel();
    bus.process();
    if (called || calledB || aw5a->isActive() || aw5b->isActive()) {
        std::cerr << "Multi awaiters cancel failed\n"; return 1;
    }

    std::cout << "All tests passed\n";
    return 0;
}
