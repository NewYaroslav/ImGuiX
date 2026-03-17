#pragma once
#ifndef _IMGUIX_CORE_THEME_VALUE_UTILS_HPP_INCLUDED
#define _IMGUIX_CORE_THEME_VALUE_UTILS_HPP_INCLUDED

/// \file theme_value_utils.hpp
/// \brief Typed helpers for resolving custom theme values from ThemeManager.

#include "ThemeManager.hpp"

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace ImGuiX::Themes {

    /// \brief Describes where a resolved theme override came from.
    enum class ThemeValueSource {
        Missing,   ///< No compatible override was found; caller fallback is kept.
        NameKey,   ///< Value came from the string key lookup.
        NumericId, ///< Value came from the numeric fallback lookup.
    };

    /// \brief Typed theme value resolved from the active theme or caller fallback.
    /// \tparam TValue Requested result type.
    template<typename TValue>
    struct ResolvedThemeValue {
        TValue value;
        ThemeValueSource source = ThemeValueSource::Missing;

        /// \brief Check whether the value came from the theme instead of fallback.
        /// \return True when a compatible theme override was resolved.
        bool hasValue() const noexcept {
            return source != ThemeValueSource::Missing;
        }
    };

    /// \brief Extract a float-compatible value from ThemeCustomValue.
    /// \param raw Raw custom theme value.
    /// \param out Output float.
    /// \return True when \p raw stores float or int.
    inline bool tryExtractThemeValue(const ThemeCustomValue& raw, float& out) noexcept {
        if (const float* value = std::get_if<float>(&raw)) {
            out = *value;
            return true;
        }
        if (const int* value = std::get_if<int>(&raw)) {
            out = static_cast<float>(*value);
            return true;
        }
        return false;
    }

    /// \brief Extract an int value from ThemeCustomValue.
    /// \param raw Raw custom theme value.
    /// \param out Output int.
    /// \return True when \p raw stores int.
    inline bool tryExtractThemeValue(const ThemeCustomValue& raw, int& out) noexcept {
        if (const int* value = std::get_if<int>(&raw)) {
            out = *value;
            return true;
        }
        return false;
    }

    /// \brief Extract a bool value from ThemeCustomValue.
    /// \param raw Raw custom theme value.
    /// \param out Output bool.
    /// \return True when \p raw stores bool.
    inline bool tryExtractThemeValue(const ThemeCustomValue& raw, bool& out) noexcept {
        if (const bool* value = std::get_if<bool>(&raw)) {
            out = *value;
            return true;
        }
        return false;
    }

    /// \brief Extract an ImVec2 value from ThemeCustomValue.
    /// \param raw Raw custom theme value.
    /// \param out Output vector.
    /// \return True when \p raw stores ImVec2.
    inline bool tryExtractThemeValue(const ThemeCustomValue& raw, ImVec2& out) noexcept {
        if (const ImVec2* value = std::get_if<ImVec2>(&raw)) {
            out = *value;
            return true;
        }
        return false;
    }

    /// \brief Extract an ImVec4 value from ThemeCustomValue.
    /// \param raw Raw custom theme value.
    /// \param out Output vector.
    /// \return True when \p raw stores ImVec4.
    inline bool tryExtractThemeValue(const ThemeCustomValue& raw, ImVec4& out) noexcept {
        if (const ImVec4* value = std::get_if<ImVec4>(&raw)) {
            out = *value;
            return true;
        }
        return false;
    }

    namespace detail {

        template<typename TValue>
        struct is_supported_theme_value_type : std::false_type {};

        template<>
        struct is_supported_theme_value_type<float> : std::true_type {};

        template<>
        struct is_supported_theme_value_type<int> : std::true_type {};

        template<>
        struct is_supported_theme_value_type<bool> : std::true_type {};

        template<>
        struct is_supported_theme_value_type<ImVec2> : std::true_type {};

        template<>
        struct is_supported_theme_value_type<ImVec4> : std::true_type {};

        template<typename TValue>
        constexpr bool is_supported_theme_value_type_v =
            is_supported_theme_value_type<typename std::decay<TValue>::type>::value;

        template<typename TValue>
        inline void static_assert_supported_theme_value_type() {
            static_assert(
                is_supported_theme_value_type_v<TValue>,
                "resolveCurrentThemeCustomValue only supports float, int, bool, ImVec2, and ImVec4");
        }

        inline ResolvedThemeValue<ImVec4> resolve_color_with_source(
                const ThemeManager& tm,
                std::string_view key,
                std::uint32_t fallback_id,
                ImVec4 fallback,
                bool use_name_key,
                bool use_numeric_id) {
            ResolvedThemeValue<ImVec4> result{fallback, ThemeValueSource::Missing};

            if (use_name_key && tm.tryGetCurrentThemeCustomColor(key, result.value)) {
                result.source = ThemeValueSource::NameKey;
                return result;
            }

            if (use_numeric_id && tm.tryGetCurrentThemeCustomColor(fallback_id, result.value)) {
                result.source = ThemeValueSource::NumericId;
                return result;
            }

            return result;
        }

        template<typename TValue>
        inline ResolvedThemeValue<TValue> resolve_value_with_source(
                const ThemeManager& tm,
                std::string_view key,
                std::uint32_t fallback_id,
                TValue fallback,
                bool use_name_key,
                bool use_numeric_id) {
            static_assert_supported_theme_value_type<TValue>();

            ResolvedThemeValue<TValue> result{fallback, ThemeValueSource::Missing};
            ThemeCustomValue raw_value;

            if (use_name_key &&
                tm.tryGetCurrentThemeCustomValue(key, raw_value) &&
                tryExtractThemeValue(raw_value, result.value)) {
                result.source = ThemeValueSource::NameKey;
                return result;
            }

            if (use_numeric_id &&
                tm.tryGetCurrentThemeCustomValue(fallback_id, raw_value) &&
                tryExtractThemeValue(raw_value, result.value)) {
                result.source = ThemeValueSource::NumericId;
                return result;
            }

            return result;
        }

    } // namespace detail

    /// \brief Resolve custom color by string key with fallback.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Custom color key.
    /// \param fallback Fallback color to keep when key is missing.
    /// \return Resolved color and its source.
    inline ResolvedThemeValue<ImVec4> resolveCurrentThemeCustomColor(
            const ThemeManager& tm,
            std::string_view key,
            ImVec4 fallback) {
        return detail::resolve_color_with_source(tm, key, 0U, fallback, true, false);
    }

    /// \brief Resolve custom color by numeric id with fallback.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Numeric custom color key.
    /// \param fallback Fallback color to keep when key is missing.
    /// \return Resolved color and its source.
    inline ResolvedThemeValue<ImVec4> resolveCurrentThemeCustomColor(
            const ThemeManager& tm,
            std::uint32_t key,
            ImVec4 fallback) {
        return detail::resolve_color_with_source(tm, {}, key, fallback, false, true);
    }

    /// \brief Resolve custom color by string key with numeric fallback.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Primary string key.
    /// \param fallback_id Numeric fallback key.
    /// \param fallback Fallback color to keep when neither key resolves.
    /// \return Resolved color and its source.
    inline ResolvedThemeValue<ImVec4> resolveCurrentThemeCustomColor(
            const ThemeManager& tm,
            std::string_view key,
            std::uint32_t fallback_id,
            ImVec4 fallback) {
        return detail::resolve_color_with_source(tm, key, fallback_id, fallback, true, true);
    }

    /// \brief Resolve custom value by string key with fallback.
    /// \tparam TValue Requested type.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Custom value key.
    /// \param fallback Fallback value to keep when key is missing or incompatible.
    /// \return Resolved value and its source.
    template<typename TValue>
    inline ResolvedThemeValue<TValue> resolveCurrentThemeCustomValue(
            const ThemeManager& tm,
            std::string_view key,
            TValue fallback) {
        return detail::resolve_value_with_source(tm, key, 0U, fallback, true, false);
    }

    /// \brief Resolve custom value by numeric id with fallback.
    /// \tparam TValue Requested type.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Numeric custom value key.
    /// \param fallback Fallback value to keep when key is missing or incompatible.
    /// \return Resolved value and its source.
    template<typename TValue>
    inline ResolvedThemeValue<TValue> resolveCurrentThemeCustomValue(
            const ThemeManager& tm,
            std::uint32_t key,
            TValue fallback) {
        return detail::resolve_value_with_source(tm, {}, key, fallback, false, true);
    }

    /// \brief Resolve custom value by string key with numeric fallback.
    /// \tparam TValue Requested type.
    /// \param tm Theme manager that owns the active theme.
    /// \param key Primary string key.
    /// \param fallback_id Numeric fallback key.
    /// \param fallback Fallback value to keep when neither key resolves compatibly.
    /// \return Resolved value and its source.
    template<typename TValue>
    inline ResolvedThemeValue<TValue> resolveCurrentThemeCustomValue(
            const ThemeManager& tm,
            std::string_view key,
            std::uint32_t fallback_id,
            TValue fallback) {
        return detail::resolve_value_with_source(tm, key, fallback_id, fallback, true, true);
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_CORE_THEME_VALUE_UTILS_HPP_INCLUDED
