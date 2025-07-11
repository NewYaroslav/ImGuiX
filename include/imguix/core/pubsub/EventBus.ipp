#include <algorithm>

namespace ImGuiX::Pubsub {

    template <typename EventType>
    void EventBus::subscribe(EventListener* owner, std::function<void(const EventType&)> callback) {
        static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");

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
        static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);
        m_event_callbacks[type].push_back(CallbackRecord{
            owner,
            std::move(callback)
        });
    }

    template <typename EventType>
    void EventBus::subscribe(EventListener* listener) {
        static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);
        auto& listener_list = m_event_listeners[type];

        if (std::find(listener_list.begin(), listener_list.end(), listener) == listener_list.end()) {
            listener_list.push_back(listener);
        }
    }

    template <typename EventType>
    void EventBus::unsubscribe(EventListener* owner) {
        static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");

        auto type = std::type_index(typeid(EventType));
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);

        auto it_cb = m_event_callbacks.find(type);
        if (it_cb != m_event_callbacks.end()) {
            auto& list = it_cb->second;
            list.erase(std::remove_if(list.begin(), list.end(),
                [owner](const CallbackRecord& rec) {
                    return rec.owner == owner;
                }), list.end());
        }

        auto it_ls = m_event_listeners.find(type);
        if (it_ls != m_event_listeners.end()) {
            auto& list = it_ls->second;
            list.erase(std::remove(list.begin(), list.end(), owner), list.end());
        }
    }
    
    inline void EventBus::unsubscribeAll(EventListener* owner) {
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);

        for (auto& [type, callback_list] : m_event_callbacks) {
            callback_list.erase(std::remove_if(callback_list.begin(), callback_list.end(),
                [owner](const CallbackRecord& rec) {
                    return rec.owner == owner;
                }), callback_list.end());
        }

        for (auto& [type, listener_list] : m_event_listeners) {
            listener_list.erase(std::remove(listener_list.begin(), listener_list.end(), owner), listener_list.end());
        }
    }

    inline void EventBus::notify(const Event* const event) const {
        auto type = std::type_index(typeid(*event));
        
        callback_list_t callbacks_copy;
        listener_list_t listeners_copy;
        
        std::unique_lock<std::mutex> lock(m_subscriptions_mutex);
        auto it = m_event_callbacks.find(type);
        if (it != m_event_callbacks.end()) {
            callbacks_copy = it->second;
        }

        auto it_listeners = m_event_listeners.find(type);
        if (it_listeners != m_event_listeners.end()) {
            listeners_copy = it_listeners->second;
        }
        lock.unlock();

        for (const auto& rec : callbacks_copy) {
            rec.callback(event);
        }

        for (auto* listener : listeners_copy) {
            listener->onEvent(event);
        }
    }

    inline void EventBus::notify(const Event& event) const {
        notify(&event);
    }

    inline void EventBus::notifyAsync(std::unique_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_event_queue.push(std::move(event));
    }

    inline void EventBus::process() {
        std::queue<std::unique_ptr<Event>> local_queue;
        
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        if (m_event_queue.empty()) return;
        std::swap(local_queue, m_event_queue);
        lock.unlock();

        while (!local_queue.empty()) {
            notify(local_queue.front().get());
            local_queue.pop();
        }
    }

} // namespace ImGuiX::Pubsub