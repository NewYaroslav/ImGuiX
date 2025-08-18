#pragma once
#ifndef _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED

/// \file LangChangeEvent.hpp
/// \brief Event requesting a language change (globally or for a specific window).

namespace ImGuiX::Events {

    /// \brief Emitted to request a language change.
    ///
    /// Применение должно выполняться «между кадрами».
    /// Если используется динамический атлас шрифтов — его перезагрузка тоже между кадрами.
    class LangChangeEvent : public Pubsub::Event {
    public:
        std::string lang;         ///< Целевой язык: "ru", "en", "uk", "pt-BR", "es", "vi".
        bool        apply_to_all; ///< true = применить ко всем окнам; false = только к указанному окну.
        int         window_id;    ///< ID окна, если apply_to_all == false. Иначе игнорируется.

        /// \brief Конструктор.
        LangChangeEvent(std::string language, bool to_all = true, int target_window_id = -1)
            : lang(std::move(language))
            , apply_to_all(to_all)
            , window_id(target_window_id) {}

        /// \brief Удобный фабричный метод: ко всем окнам.
        static LangChangeEvent ForAll(std::string language) {
            return LangChangeEvent(std::move(language), /*to_all=*/true, /*target_window_id=*/-1);
        }

        /// \brief Удобный фабричный метод: только к одному окну.
        static LangChangeEvent ForWindow(std::string language, int target_window_id) {
            return LangChangeEvent(std::move(language), /*to_all=*/false, target_window_id);
        }

        std::type_index type() const override {
            return typeid(LangChangeEvent);
        }

        const char* name() const override {
            return "LangChangeEvent";
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_LANG_CHANGE_EVENT_HPP_INCLUDED
