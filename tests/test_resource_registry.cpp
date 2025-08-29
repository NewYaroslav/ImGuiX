#include <iostream>
#include <string>
#include "imguix/core/resource/ResourceRegistry.hpp"

using namespace ImGuiX;

struct DummyResource {
    std::string name;
};

int main() {
    ResourceRegistry registry;

    // Register a resource
    bool registered = registry.registerResource<DummyResource>([]() {
        return std::make_unique<DummyResource>(DummyResource{"MyResource"});
    });

    std::cout << "Resource registered: " << std::boolalpha << registered << "\n";

    // Attempt to retrieve the resource
    auto& res = registry.getResource<DummyResource>();
    std::cout << "Resource retrieved: " << res.name << "\n";
    
    auto res2 = registry.tryGetResource<DummyResource>();
    if (res2) {
        std::cout << "Resource retrieved: " << res2->get().name << "\n";
    } else {
        std::cout << "Resource not found!\n";
    }

    // Test duplicate registration
    bool duplicate = registry.registerResource<DummyResource>([] {
        return std::make_unique<DummyResource>(DummyResource{"Duplicate"});
    });

    std::cout << "Duplicate registration result: " << std::boolalpha << duplicate << "\n";
    std::cin.get();
    return 0;
}