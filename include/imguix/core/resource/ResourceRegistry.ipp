namespace ImGuiX {

    inline void ResourceRegistry::clearAll() {
        std::unique_lock progress_lock(m_progress_mutex, std::defer_lock);
        std::unique_lock resource_lock(m_resources_mutex, std::defer_lock);
        std::lock(progress_lock, resource_lock);

        m_resources.clear();
        m_in_progress.clear();
    }

} // namespace ImGuiX
