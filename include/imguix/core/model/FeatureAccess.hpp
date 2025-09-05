#pragma once
#ifndef _IMGUIX_CORE_MODEL_FEATURE_ACCESS_HPP_INCLUDED
#define _IMGUIX_CORE_MODEL_FEATURE_ACCESS_HPP_INCLUDED

/// \file FeatureAccess.hpp
/// \brief Helper mixin for controllers to access local feature models.

#include <functional>
#include <memory>
#include <utility>

#include "LocalModelRegistry.hpp"

namespace ImGuiX {

    namespace model {

        /// \brief Mixin enabling controllers to manage feature models.
        class FeatureAccessMixin {
        protected:
            LocalModelRegistry m_features; ///< Stored feature models.

            /// \brief Read-only access to a feature model, creating on first use.
            /// \tparam M Feature model type deriving from ILocalModel.
            /// \tparam Factory Callable returning std::unique_ptr<M>.
            /// \param factory Factory to create the model.
            /// \return Const reference to the feature model.
            template<class M, class Factory>
            const M& feature(Factory&& factory) const {
                return const_cast<FeatureAccessMixin*>(this)
                    ->m_features.template getOrCreate<M>(std::forward<Factory>(factory));
            }

            /// \brief Mutate or act on a feature model on the UI thread.
            /// \tparam M Feature model type.
            /// \tparam Factory Callable returning std::unique_ptr<M>.
            /// \tparam Fn Callable taking M& and returning any type.
            /// \param factory Factory to create the model.
            /// \param fn Function applied to the model.
            /// \return Result of fn.
            template<class M, class Factory, class Fn>
            decltype(auto) withFeature(Factory&& factory, Fn&& fn) {
                M& ref = m_features.template getOrCreate<M>(std::forward<Factory>(factory));
                return std::forward<Fn>(fn)(ref);
            }

            /// \brief Check whether a feature model exists.
            /// \tparam M Feature model type.
            /// \return True if model exists.
            template<class M>
            bool hasFeature() const {
                return m_features.template tryGet<M>() != nullptr;
            }

            /// \brief Remove feature model if present.
            /// \tparam M Feature model type.
            template<class M>
            void resetFeature() {
                m_features.template reset<M>();
            }

            /// \brief Process all feature models for the current frame.
            /// \tparam Sync SyncNotifier type.
            /// \param sync Notifier passed to each model.
            template<class Sync>
            void processFeatures(Sync& sync) {
                m_features.processAll(sync);
            }
        };

    } // namespace model

} // namespace ImGuiX

#endif // _IMGUIX_CORE_MODEL_FEATURE_ACCESS_HPP_INCLUDED
