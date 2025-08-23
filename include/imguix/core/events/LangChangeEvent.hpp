#pragma once
#ifndef _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED

/// \file LangChangeEvent.hpp
/// \brief Event requesting a language change (globally or for a specific window).

namespace ImGuiX::Events {

    /// \brief Emitted to request a language change.
    /// Should be applied between frames.
    /// When using a dynamic font atlas, reload it between frames as well.
    class LangChangeEvent : public Pubsub::Event {
    public:
        std::string lang;         ///< Target language code such as "ru", "en", "uk", "pt-BR", "es", or "vi".
        bool        apply_to_all; ///< true to apply to all windows; false to apply only to a specific window.
        int         window_id;    ///< ID of the target window when apply_to_all is false; ignored otherwise.

        /// \brief Constructs a language change event.
        /// \param language Language code to set.
        /// \param to_all Apply to all windows if true.
        /// \param target_window_id ID of the target window when to_all is false.
        LangChangeEvent(std::string language, bool to_all = true, int target_window_id = -1)
            : lang(std::move(language))
            , apply_to_all(to_all)
            , window_id(target_window_id) {}

        /// \brief Factory helper that targets all windows.
        static LangChangeEvent ForAll(std::string language) {
            return LangChangeEvent(std::move(language), /*to_all=*/true, /*target_window_id=*/-1);
        }

        /// \brief Factory helper that targets a single window.
        /// \param language Language code to set.
        /// \param target_window_id ID of the window that should change its language.
        static LangChangeEvent ForWindow(std::string language, int target_window_id) {
            return LangChangeEvent(std::move(language), /*to_all=*/false, target_window_id);
        }

        std::type_index type() const override {
            return typeid(LangChangeEvent);
        }

        const char* name() const override {
            return u8"LangChangeEvent";
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED
