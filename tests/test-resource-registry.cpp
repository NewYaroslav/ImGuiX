#include <iostream>
#include <string>
#include "imguix/core/ResourceRegistry.hpp"

using namespace ImGuiX;

struct DummyResource {
    std::string name;
};

int main() {
    ResourceRegistry registry;

    // Регистрация ресурса
    bool registered = registry.registerResource<DummyResource>([]() {
        return std::make_unique<DummyResource>(DummyResource{"MyResource"});
    });

    std::cout << "Resource registered: " << std::boolalpha << registered << "\n";

    // Попытка получить ресурс
    auto& res = registry.getResource<DummyResource>();
    std::cout << "Resource retrieved: " << res.name << "\n";
    
    auto res2 = registry.tryGetResource<DummyResource>();
    if (res2) {
        std::cout << "Resource retrieved: " << res2->get().name << "\n";
    } else {
        std::cout << "Resource not found!\n";
    }

    // Проверка повторной регистрации
    bool duplicate = registry.registerResource<DummyResource>([] {
        return std::make_unique<DummyResource>(DummyResource{"Duplicate"});
    });

    std::cout << "Duplicate registration result: " << std::boolalpha << duplicate << "\n";
    std::cin.get();
    return 0;
}