#pragma once
#ifndef _IMGUIX_CORE_MODEL_ILOCAL_MODEL_HPP_INCLUDED
#define _IMGUIX_CORE_MODEL_ILOCAL_MODEL_HPP_INCLUDED

/// \file ILocalModel.hpp
/// \brief Interface for lightweight controller-local models without ImGui calls.

namespace ImGuiX {

    namespace Pubsub {
        class SyncNotifier;
    }

    namespace model {

        /// \brief Interface for controller-owned models processed each frame.
        struct ILocalModel {
            virtual ~ILocalModel() = default;

            /// \brief Called once when the model is added to the controller registry.
            virtual void onAttach() {}

            /// \brief Called once before the model is removed from the controller registry.
            virtual void onDetach() {}

            /// \brief Per-frame hook executed on the UI thread.
            /// \param sync Synchronous notifier bound to the event bus.
            virtual void process(Pubsub::SyncNotifier& sync) { (void)sync; }
        };

    } // namespace model

} // namespace ImGuiX

#endif // _IMGUIX_CORE_MODEL_ILOCAL_MODEL_HPP_INCLUDED
