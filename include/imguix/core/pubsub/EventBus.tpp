namespace ImGuiX::Pubsub {

    template <typename EventType>
    void EventBus::subscribe(EventListener* owner, std::function<void(const EventType&)> callback) {
        static_assert(std::is_base_of<Event, EventType>::value, u8"EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);
        m_event_callbacks[type].push_back(CallbackRecord{
            owner,
            [callback = std::move(callback)](const Event* const e) {
                callback(*static_cast<const EventType*>(e));
            }
        });
    }

    template <typename EventType>
    void EventBus::subscribe(EventListener* owner, std::function<void(const Event* const)> callback) {
        static_assert(std::is_base_of<Event, EventType>::value, u8"EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);
        m_event_callbacks[type].push_back(CallbackRecord{
            owner,
            std::move(callback)
        });
    }

    template <typename EventType>
    void EventBus::subscribe(EventListener* listener) {
        static_assert(std::is_base_of<Event, EventType>::value, u8"EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);
        auto& listener_list = m_event_listeners[type];

        if (std::find(listener_list.begin(), listener_list.end(), listener) == listener_list.end()) {
            listener_list.push_back(listener);
        }
    }

    template <typename EventType>
    void EventBus::unsubscribe(EventListener* owner) {
        static_assert(std::is_base_of<Event, EventType>::value, u8"EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);

        auto it_cb = m_event_callbacks.find(type);
        if (it_cb != m_event_callbacks.end()) {
            auto& list = it_cb->second;
            list.erase(std::remove_if(list.begin(), list.end(),
                [owner](const CallbackRecord& rec) {
                    return rec.owner == owner;
                }), list.end());
            if (list.empty()) {
                m_event_callbacks.erase(it_cb);
            }
        }

        auto it_ls = m_event_listeners.find(type);
        if (it_ls != m_event_listeners.end()) {
            auto& list = it_ls->second;
            list.erase(std::remove(list.begin(), list.end(), owner), list.end());
            if (list.empty()) {
                m_event_listeners.erase(it_ls);
            }
        }
    }

} // namespace ImGuiX::Pubsub
