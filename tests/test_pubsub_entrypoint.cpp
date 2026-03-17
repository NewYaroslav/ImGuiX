#include <imguix/core/pubsub.hpp>

#include <memory>
#include <typeindex>

namespace {

    struct PubsubEntrypointEvent final : ImGuiX::Pubsub::Event {
        std::type_index type() const override {
            return typeid(PubsubEntrypointEvent);
        }

        const char* name() const override {
            return "PubsubEntrypointEvent";
        }

        std::unique_ptr<ImGuiX::Pubsub::Event> clone() const override {
            return std::make_unique<PubsubEntrypointEvent>(*this);
        }
    };

} // namespace

int main() {
    ImGuiX::Pubsub::EventBus bus;

    bus.notifyAsync(std::make_unique<PubsubEntrypointEvent>());
    bus.process();
    bus.unsubscribeAll(nullptr);

    return 0;
}
