#include <imguix/config/build.hpp>

namespace ImGuiX::Pubsub {

    IMGUIX_IMPL_INLINE void EventBus::unsubscribeAll(EventListener* owner) {
        std::lock_guard<std::mutex> lock(m_subscriptions_mutex);

        for (auto it = m_event_callbacks.begin(); it != m_event_callbacks.end(); ) {
            auto& callback_list = it->second;
            callback_list.erase(std::remove_if(callback_list.begin(), callback_list.end(),
                [owner](const CallbackRecord& rec) {
                    return rec.owner == owner;
                }), callback_list.end());
            if (callback_list.empty()) {
                it = m_event_callbacks.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = m_event_listeners.begin(); it != m_event_listeners.end(); ) {
            auto& listener_list = it->second;
            listener_list.erase(std::remove(listener_list.begin(), listener_list.end(), owner), listener_list.end());
            if (listener_list.empty()) {
                it = m_event_listeners.erase(it);
            } else {
                ++it;
            }
        }
    }

    IMGUIX_IMPL_INLINE void EventBus::notify(const Event* const event) const {
        auto type = event->type();
        
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

    IMGUIX_IMPL_INLINE void EventBus::notify(const Event& event) const {
        notify(&event);
    }

    IMGUIX_IMPL_INLINE void EventBus::notifyAsync(std::unique_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_event_queue.push(std::move(event));
    }

    IMGUIX_IMPL_INLINE void EventBus::registerAwaiter(const std::shared_ptr<IAwaiterEx>& aw) {
        std::lock_guard<std::mutex> lk(m_awaiters_mutex);
        m_awaiters.emplace_back(aw);
    }

    IMGUIX_IMPL_INLINE void EventBus::pollAwaitersInternal() {
        std::vector<std::shared_ptr<IAwaiterEx>> live;
        {
            std::lock_guard<std::mutex> lk(m_awaiters_mutex);
            auto& v = m_awaiters;
            v.erase(std::remove_if(v.begin(), v.end(), [](const std::weak_ptr<IAwaiterEx>& w){
                auto sp = w.lock();
                if (!sp) return true;
                return !sp->isActive();
            }), v.end());
            live.reserve(v.size());
            for (auto& w : v) {
                if (auto sp = w.lock()) live.emplace_back(std::move(sp));
            }
        }
        for (auto& aw : live) aw->pollTimeout();
    }

    IMGUIX_IMPL_INLINE void EventBus::process() {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        if (m_event_queue.empty()) {
            lock.unlock();
            pollAwaitersInternal();
            return;
        }

        std::queue<std::unique_ptr<Event>> local_queue;
        std::swap(local_queue, m_event_queue);
        lock.unlock();

        while (!local_queue.empty()) {
            notify(local_queue.front().get());
            local_queue.pop();
        }

        pollAwaitersInternal();
    }

} // namespace ImGuiX::Pubsub
