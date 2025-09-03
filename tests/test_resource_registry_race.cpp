#include <atomic>
#include <future>
#include <memory>
#include <thread>

#include "imguix/core/resource/ResourceRegistry.hpp"

using namespace ImGuiX;

struct DummyResource {
    int value = 7;
};

int main() {
    ResourceRegistry registry;
    std::promise<void> start_promise;
    std::promise<void> finish_promise;
    auto start_future = start_promise.get_future();
    auto finish_future = finish_promise.get_future();

    std::thread reg([&] {
        registry.registerResource<DummyResource>([&] {
            start_promise.set_value();
            finish_future.wait();
            return std::make_shared<DummyResource>();
        });
    });

    start_future.wait();

    std::atomic<bool> failed{false};
    std::thread getter([&] {
        try {
            DummyResource& res = registry.getResource<DummyResource>();
            if (res.value != 7) {
                failed = true;
            }
        } catch (...) {
            failed = true;
        }
    });

    finish_promise.set_value();

    reg.join();
    getter.join();

    return failed ? 1 : 0;
}
