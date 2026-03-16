#pragma once

#if defined(__EMSCRIPTEN__)
#   include <mutex>
#else
#   include <mutex>
#   include <shared_mutex>
#endif

namespace ImGuiX {

    template <typename T>
    bool ResourceRegistry::registerResource(std::function<std::shared_ptr<T>()> creator) {
        const std::type_index type = typeid(T);

        std::scoped_lock lock(m_progress_mutex, m_resources_mutex);
        if (!m_in_progress.insert(type).second) {
            return false;
        }
        if (m_resources.find(type) != m_resources.end()) {
            m_in_progress.erase(type);
            return false;
        }
        try {
            m_resources[type] = creator();
            m_in_progress.erase(type);
            return true;
        } catch (...) {
            m_in_progress.erase(type);
            throw;
        }
    }

    template <typename T>
    bool ResourceRegistry::registerResource() {
        return registerResource<T>([] { return std::make_shared<T>(); });
    }

    template <typename T>
    T& ResourceRegistry::getResource() {
        const std::type_index type = typeid(T);

#if defined(__EMSCRIPTEN__)
        std::unique_lock<std::mutex> progress_lock(m_progress_mutex);
#else
        std::shared_lock<std::shared_mutex> progress_lock(m_progress_mutex);
#endif
        if (m_in_progress.find(type) != m_in_progress.end()) {
            throw std::runtime_error(u8"Resource is being initialized");
        }
        progress_lock.unlock();

#if defined(__EMSCRIPTEN__)
        std::unique_lock<std::mutex> resource_lock(m_resources_mutex);
#else
        std::shared_lock<std::shared_mutex> resource_lock(m_resources_mutex);
#endif
        auto it = m_resources.find(type);
        if (it == m_resources.end()) {
            throw std::runtime_error(u8"Resource not registered");
        }

        return *static_cast<T*>(it->second.get());
    }

    template <typename T>
    std::optional<std::reference_wrapper<T>> ResourceRegistry::tryGetResource() {
        const std::type_index type = typeid(T);

#if defined(__EMSCRIPTEN__)
        std::unique_lock<std::mutex> progress_lock(m_progress_mutex);
#else
        std::shared_lock<std::shared_mutex> progress_lock(m_progress_mutex);
#endif
        if (m_in_progress.find(type) != m_in_progress.end()) {
            return std::nullopt;
        }
        progress_lock.unlock();

#if defined(__EMSCRIPTEN__)
        std::unique_lock<std::mutex> resource_lock(m_resources_mutex);
#else
        std::shared_lock<std::shared_mutex> resource_lock(m_resources_mutex);
#endif
        auto it = m_resources.find(type);
        if (it == m_resources.end()) {
            return std::nullopt;
        }

        return std::ref(*static_cast<T*>(it->second.get()));
    }

} // namespace ImGuiX
