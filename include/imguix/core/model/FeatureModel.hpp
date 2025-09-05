#pragma once
#ifndef _IMGUIX_CORE_MODEL_FEATURE_MODEL_HPP_INCLUDED
#define _IMGUIX_CORE_MODEL_FEATURE_MODEL_HPP_INCLUDED

/// \file FeatureModel.hpp
/// \brief Base class for controller-local models providing event mediation.

#include <atomic>
#include "ILocalModel.hpp"

namespace ImGuiX {

    namespace Pubsub {
        class EventBus;
    }

    namespace model {

        /// \brief Base class for feature-local models owned by a controller.
        /// \note Avoid direct ImGui calls; all interactions occur on the UI thread.
        class FeatureModel : public ILocalModel, public Pubsub::EventMediator {
        public:
            /// \brief Construct feature model bound to an event bus.
            /// \param bus Event bus for subscriptions.
            explicit FeatureModel(Pubsub::EventBus& bus) : Pubsub::EventMediator(bus) {}

            FeatureModel(const FeatureModel&) = delete;
            FeatureModel& operator=(const FeatureModel&) = delete;
            FeatureModel(FeatureModel&&) = delete;
            FeatureModel& operator=(FeatureModel&&) = delete;

            ~FeatureModel() override = default;

            /// \brief Request shutdown for background workers owned by this feature.
            void requestClose() { m_is_closing.store(true, std::memory_order_relaxed); }

            /// \brief Check whether shutdown was requested.
            /// \return True if closing is requested.
            bool isClosing() const { return m_is_closing.load(std::memory_order_relaxed); }

            void onAttach() override {}
            void onDetach() override {}

            /// \brief Per-frame hook executed on the UI thread.
            /// \param sync Synchronous notifier bound to the event bus.
            void process(Pubsub::SyncNotifier& sync) override { (void)sync; }

        private:
            std::atomic<bool> m_is_closing{false};
        };

    } // namespace model

} // namespace ImGuiX

#endif // _IMGUIX_CORE_MODEL_FEATURE_MODEL_HPP_INCLUDED
