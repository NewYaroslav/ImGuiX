#pragma once
#ifndef _IMGUIX_CORE_MODEL_LOCAL_MODEL_REGISTRY_HPP_INCLUDED
#define _IMGUIX_CORE_MODEL_LOCAL_MODEL_REGISTRY_HPP_INCLUDED

/// \file LocalModelRegistry.hpp
/// \brief Type-safe storage for controller-owned feature models.

#include <memory>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ILocalModel.hpp"

namespace ImGuiX {

    namespace model {

        /// \brief Registry mapping model types to unique instances.
        class LocalModelRegistry {
        public:
            LocalModelRegistry() = default;
            ~LocalModelRegistry() { clear(); }

            /// \brief Get existing model or create a new one using factory.
            /// \tparam M Model type deriving from ILocalModel.
            /// \tparam Factory Callable returning std::unique_ptr<M>.
            /// \param factory Factory to create the model on first use.
            /// \return Reference to the model instance.
            template<class M, class Factory>
            M& getOrCreate(Factory&& factory) {
                static_assert(std::is_base_of<ILocalModel, M>::value,
                              "M must implement ILocalModel");
                const std::type_index key{typeid(M)};
                auto it = m_models.find(key);
                if (it == m_models.end()) {
                    auto up = factory();
                    M* raw = up.get();
                    raw->onAttach();
                    m_models.emplace(key, std::move(up));
                    return *raw;
                }
                return *static_cast<M*>(it->second.get());
            }

            /// \brief Try to get existing model.
            /// \tparam M Model type.
            /// \return Pointer to model or nullptr.
            template<class M>
            const M* tryGet() const {
                auto it = m_models.find(std::type_index{typeid(M)});
                return it == m_models.end() ? nullptr : static_cast<M*>(it->second.get());
            }

            /// \brief Remove model of type M if present.
            /// \tparam M Model type.
            template<class M>
            void reset() {
                auto it = m_models.find(std::type_index{typeid(M)});
                if (it != m_models.end()) {
                    it->second->onDetach();
                    m_models.erase(it);
                }
            }

            /// \brief Remove all models and invoke onDetach().
            void clear() {
                for (auto& kv : m_models) kv.second->onDetach();
                m_models.clear();
            }

            /// \brief Invoke process(sync) on all stored models.
            /// \tparam Sync SyncNotifier type.
            /// \param sync Notifier passed to each model.
            template<class Sync>
            void processAll(Sync& sync) {
                for (auto& kv : m_models) kv.second->process(sync);
            }

        private:
            std::unordered_map<std::type_index, std::unique_ptr<ILocalModel>> m_models;
        };

    } // namespace model

} // namespace ImGuiX

#endif // _IMGUIX_CORE_MODEL_LOCAL_MODEL_REGISTRY_HPP_INCLUDED
