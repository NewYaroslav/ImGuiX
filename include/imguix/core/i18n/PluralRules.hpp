#pragma once
#ifndef _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED
#define _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED

/// \file PluralRules.hpp
/// \brief Pluggable plural rules: built-in fallback (en/ru) + JSON file loader.
/// JSON format (see docs): per-language "cardinal" array of rules with conditions.

namespace ImGuiX::I18N {

    namespace fs = std::filesystem;

    /// \class PluralRules
    /// \brief Runtime-pluggable pluralization rules with JSON loading.
    class PluralRules {
    public:
        /// \brief Load rules from a JSON file.
        /// \param path Path to JSON file.
        /// \return True on success.
        bool load_from_file(const std::string& path);

        /// \brief Determine plural category for number in given language.
        /// \param lang Language code (e.g., "en").
        /// \param n Numeric value.
        /// \return Category string such as "one" or "other".
        /// \note Built-in fallback supports English (one/other) and Russian (one/few/many/other).
        std::string category(const std::string& lang, long long n) const;

    private:
        struct Rule {
            std::string category;
            nlohmann::json cond; // condition object
        };
        struct LangRules {
            std::vector<Rule> rules;
        };
        std::unordered_map<std::string, LangRules> rules_;

        static std::string read_file(const std::string& path);

        // --- evaluator ---

        static bool eval_rule(const nlohmann::json& r, long long n);
    };

} // namespace ImGuiX::I18N

#ifdef IMGUIX_HEADER_ONLY
#   include "PluralRules.ipp"
#endif

#endif // _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED
