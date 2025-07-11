#include <stdexcept>
#include <mutex>

namespace ImGuiX {

    template <typename T>
    bool ResourceRegistry::registerResource(std::function<std::shared_ptr<T>()> creator) {
        const std::type_index type = typeid(T);
        
        std::unique_lock progress_lock(m_progress_mutex);
        if (!m_in_progress.insert(type).second) {
            return false; // уже в процессе
        }
        progress_lock.unlock();
        
        std::unique_lock resource_lock(m_resources_mutex);
        if (m_resources.find(type) != m_resources.end()) return false;
        try {
            m_resources[type] = creator();
			resource_lock.unlock();
            std::unique_lock lock(m_progress_mutex);
            m_in_progress.erase(type);
            return true;
        } catch (...) {
            resource_lock.unlock();
			std::unique_lock lock(m_progress_mutex);
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
        
        std::shared_lock progress_lock(m_progress_mutex);
        if (m_in_progress.find(type) != m_in_progress.end()) {
            throw std::runtime_error("Resource is being initialized");
        }
        progress_lock.unlock();
        
        std::shared_lock resource_lock(m_resources_mutex);
        auto it = m_resources.find(type);
        if (it == m_resources.end()) {
            throw std::runtime_error("Resource not registered");
        }

        return *static_cast<T*>(it->second.get());
    }

    template <typename T>
    std::optional<std::reference_wrapper<T>> ResourceRegistry::tryGetResource() {
        const std::type_index type = typeid(T);
        
        std::shared_lock progress_lock(m_progress_mutex);
        if (m_in_progress.find(type) != m_in_progress.end()) {
            return std::nullopt;
        }
        progress_lock.unlock();
        
        
        std::shared_lock resource_lock(m_resources_mutex);
        auto it = m_resources.find(type);
        if (it == m_resources.end()) {
            return std::nullopt;
        }

        return std::ref(*static_cast<T*>(it->second.get()));
    }

    inline void ResourceRegistry::clearAll() {
        std::unique_lock progress_lock(m_progress_mutex, std::defer_lock);
        std::unique_lock resource_lock(m_resources_mutex, std::defer_lock);
        std::lock(progress_lock, resource_lock);

        m_resources.clear();
        m_in_progress.clear();
    }

} // namespace ImGuiX