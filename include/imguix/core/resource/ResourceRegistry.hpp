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
#include <shared_mutex>
#include <optional>
#include <functional>

namespace ImGuiX {

    /// \class ResourceRegistry
    /// \brief Manages registration and access to shared resources in a threadsafe manner.
    ///
    /// The ResourceRegistry stores type-erased shared resources and allows
    /// registration, retrieval, and cleanup. It ensures only one instance of each type
    /// is created and supports both default and custom construction.
    ///
    /// Access to a resource in the middle of registration throws or returns nullopt.
    class ResourceRegistry {
    public:

        /// \brief Registers a resource using a custom creator function.
        /// \tparam T Resource type.
        /// \param creator Function that returns a shared_ptr to T.
        /// \return true if the resource was registered, false if already exists.
        template <typename T>
        bool registerResource(std::function<std::shared_ptr<T>()> creator);

        /// \brief Registers a resource using default constructor.
        /// \tparam T Resource type.
        /// \return true if the resource was registered, false if already exists.
        template <typename T>
        bool registerResource();

        /// \brief Retrieves a reference to a registered resource.
        /// \tparam T Resource type.
        /// \return Reference to the resource.
        /// \throws std::runtime_error if not registered or still initializing.
        template <typename T>
        [[nodiscard]]
        T& getResource();

        /// \brief Tries to get a reference to a registered resource.
        /// \tparam T Resource type.
        /// \return std::optional containing reference if available.
        template <typename T>
        [[nodiscard]]
        std::optional<std::reference_wrapper<T>> tryGetResource();

        /// \brief Clears all registered resources.
        void clearAll();

    private:
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources; ///< Stored resources by type.
        std::unordered_set<std::type_index> m_in_progress; ///< Types currently being created.

        mutable std::shared_mutex m_resources_mutex; ///< Protects resource map.
        mutable std::shared_mutex m_progress_mutex;  ///< Protects initialization set.
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#include "ResourceRegistry.ipp"
#endif

#endif // _IMGUIX_CORE_RESOURCE_REGISTRY_HPP_INCLUDED