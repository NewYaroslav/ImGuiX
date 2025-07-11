namespace ImGuiX {

    Application::Application()
        : m_event_bus(), m_registry(), 
          m_window_manager(*static_cast<ApplicationControl*>(this)) {}

    void Application::run(bool async) {
#       ifdef IMGUIX_USE_SFML_BACKEND
        if (!registry().registerResource<DeltaClockSfml>()) {
            throw std::runtime_error("Failed to register DeltaClockSfml resource");
        }
#       endif
        if (async) {
            m_main_thread = std::thread([this]() {
#               ifdef IMGUIX_USE_SFML_BACKEND
                registry().getResource<DeltaClockSfml>().update();
#               endif
                this->mainLoop();
            });
        } else {
#           ifdef IMGUIX_USE_SFML_BACKEND
            registry().getResource<DeltaClockSfml>().update();
#           endif
            mainLoop();
        }
    }

    template<typename T, typename... Args>
    T& Application::createWindow(Args&&... args) {
        static_assert(std::is_base_of<WindowInstance, T>::value,
                      "T must derive from WindowInstance");

        int id = m_next_window_id++;

        auto window = std::make_unique<T>(id, *this,
                                          std::forward<Args>(args)...);
        T& ref = *window;

        m_window_manager.addWindow(std::move(window));
        return ref;
    }
    
    template<typename T, typename... Args>
    T& Application::createModel(Args&&... args) {
        static_assert(std::is_base_of<Model, T>::value, "T must be derived from Model");
        auto model = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T& ref = *model;
        m_pending_models.push_back(model.get());
        m_models.emplace_back(std::move(model));
        return ref;
    }

    void Application::close() {
        auto async_event = std::make_unique<Events::ApplicationExitEvent>();
        m_event_bus.notifyAsync(std::move(async_event));
    }

    bool Application::isClosing() const {
        return m_is_closing;
    }

    Pubsub::EventBus& Application::eventBus() {
        return m_event_bus;
    }

    ResourceRegistry& Application::registry() {
        return m_registry;
    }
    
    const std::string& Application::name() const {
        return m_app_name;
    }

    bool Application::allWindowsClosed() const {
        return m_window_manager.allWindowsClosed();
    }

    void Application::initializePendingModels() {
        for (auto* model : m_pending_models) {
            if (model) model->onInit();
        }
        m_pending_models.clear();
    }

    void Application::mainLoop() {
        m_window_manager.flushPending();
        m_window_manager.initializePending();
        initializePendingModels();
        while (true) {
            m_window_manager.flushPending();

            m_window_manager.initializePending();
            initializePendingModels();

            m_window_manager.removeClosed();
            if (allWindowsClosed()) {
                break;
            }

            for (auto& model : m_models) {
                model->process();
            }
            
            m_event_bus.process();

            m_window_manager.handleEvents();
            m_window_manager.tickAll();
            m_window_manager.drawContentAll();
            m_window_manager.drawUiAll();
            m_window_manager.presentAll();
        }
        m_is_closing = true;
    }
} // namespace ImGuiX

