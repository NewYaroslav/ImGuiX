#pragma once
#ifndef _IMGUIX_CORE_RESOURCE_REGISTRY_HPP_INCLUDED
#define _IMGUIX_CORE_RESOURCE_REGISTRY_HPP_INCLUDED

/// \file ResourceRegistry.hpp
/// \brief Provides a centralized resource registry for managing shared resources.
/// \ingroup Core

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <typeindex>
#if defined(__EMSCRIPTEN__)
#   include <mutex>
#else
#   include <shared_mutex>
#endif
#include <optional>
#include <functional>

namespace ImGuiX {

    /// \class ResourceRegistry
    /// \brief Manage registration and access to shared resources in a threadsafe manner.
    /// \note Stores type-erased shared resources and ensures only one instance per type.
    /// \note Access during registration throws or returns `nullopt`.
    class ResourceRegistry {
    public:

        /// \brief Register a resource using a custom creator function.
        /// \tparam T Resource type.
        /// \param creator Function returning `shared_ptr<T>`.
        /// \return True if resource was registered, false if already exists.
        template <typename T>
        bool registerResource(std::function<std::shared_ptr<T>()> creator);

        /// \brief Register a resource using default constructor.
        /// \tparam T Resource type.
        /// \return True if resource was registered, false if already exists.
        template <typename T>
        bool registerResource();

        /// \brief Get reference to registered resource.
        /// \tparam T Resource type.
        /// \return Reference to resource.
        /// \throws std::runtime_error if not registered or still initializing.
        template <typename T>
        [[nodiscard]]
        T& getResource();

        /// \brief Try to get reference to a registered resource.
        /// \tparam T Resource type.
        /// \return Optional reference if available.
        template <typename T>
        [[nodiscard]]
        std::optional<std::reference_wrapper<T>> tryGetResource();

        /// \brief Clear all registered resources.
        void clearAll();

    private:
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources; ///< Stored resources by type.
        std::unordered_set<std::type_index> m_in_progress; ///< Types currently being created.

        #if defined(__EMSCRIPTEN__)
        mutable std::mutex m_resources_mutex; ///< Protects resource map.
        mutable std::mutex m_progress_mutex;  ///< Protects initialization set.
        #else
        mutable std::shared_mutex m_resources_mutex; ///< Protects resource map.
        mutable std::shared_mutex m_progress_mutex;  ///< Protects initialization set.
        #endif
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "ResourceRegistry.ipp"
#endif

#endif // _IMGUIX_CORE_RESOURCE_REGISTRY_HPP_INCLUDED