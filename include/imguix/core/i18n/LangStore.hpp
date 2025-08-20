#pragma once
#ifndef _IMGUIX_UTILS_I18N_LANG_STORE_HPP_INCLUDED
#define _IMGUIX_UTILS_I18N_LANG_STORE_HPP_INCLUDED

/// \file LangStore.hpp
/// \brief Localized strings and markdown loader with JSON (monolingual + polyglot) support,
///        using pluggable plural rules (PluralRules).
///
/// Features:
/// - JSON loader supports:
///    1) monolingual:  { "Key": "Text", ... }
///    2) polyglot:     { "Key": { "en":"...", "ru":"...", ... }, ... }
///   JSON is preprocessed with strip_json_comments(...) before parsing.
/// - Markdown: *.md files are monolingual (per-language folder), lazy-loaded with cache.
/// - Fallback: current_lang -> default_lang ("en").
/// - Formatting: fmt-based textf_*() helpers.
/// - ImGui ID: cached "label##id" via label(key) (lazy init per key+lang).
/// - Pluralization: via PluralRules (JSON-driven, with built-ins in the rules).
///
/// Layout:
///   <base_dir>/<lang>/*.json   (short strings)
///   <base_dir>/<lang>/*.md     (long texts)
///   <base_dir>/plurals.json    (optional; plural rules)
///
/// Notes:
/// - Not thread-safe by design; guard externally if needed.
/// - All caches are invalidated on language switch.

namespace ImGuiX::Utils::I18N {

    namespace fs = std::filesystem;

    class LangStore {
    public:
        using StrMap = std::unordered_map<std::string, std::string>;
        
        LangStore()
            : LangStore(default_i18n_base_dir(), "en")
        {}

        /// \brief Construct i18n store.
        /// \param base_dir Root folder with per-language subfolders (e.g., "<root>/en/", "<root>/ru/").
        /// \param default_lang Default language (fallback), typically "en".
        explicit LangStore(std::string base_dir, std::string default_lang = "en")
            : m_base_dir(std::move(base_dir))
            , m_default_lang(std::move(default_lang))
            , m_current_lang(m_default_lang)
            , m_plural_rules(std::make_unique<PluralRules>()) // always present; has built-ins
        {
            // Load fallback language (default) once.
            m_en_map = load_language_map(m_default_lang);
            m_current_map = &m_en_map;

            // Try optional plurals.json in <base_dir>/plurals.json (ignore if absent).
            try_load_plural_rules_from_default_location();
        }

        /// \brief Change current language; rebuild language caches.
        void set_language(std::string lang) {
            if (lang == m_current_lang) return;
            m_current_lang = std::move(lang);

            if (m_current_lang == m_default_lang) {
                m_current_map = &m_en_map;
            } else {
                // build or reuse from cache
                auto it = m_lang_cache.find(m_current_lang);
                if (it == m_lang_cache.end()) {
                    auto map = load_language_map(m_current_lang);
                    it = m_lang_cache.emplace(m_current_lang, std::move(map)).first;
                }
                m_current_map = &it->second;
            }

            // Reset per-language caches
            m_label_cache.clear();
            m_md_cache.clear();
        }

        /// \brief Current language code (e.g., "ru", "en").
        const std::string& language() const noexcept { return m_current_lang; }

        /// \brief Set external plural rules implementation (takes ownership).
        void set_plural_rules(std::unique_ptr<PluralRules> rules) {
            if (rules) m_plural_rules = std::move(rules);
        }

        /// \brief Try to load plural rules from a JSON file (returns true on success).
        bool load_plural_rules_from_file(const std::string& path) {
            return m_plural_rules ? m_plural_rules->load_from_file(path) : false;
        }

        /// \brief Plain localized text for a key with fallback to default language.
        /// \return Reference to internal storage; invalidated on language switch or map reload.
        const std::string& text(std::string_view key) const {
            if (const auto* s = find_in(*m_current_map, key)) return *s;
            if (const auto* s = find_in(m_en_map, key))       return *s;
            return missing_string();
        }

        /// \brief Formatted text with compile-time checked format string.
        template <typename... Args>
        std::string textf(fmt::format_string<Args...> fmt_str, Args&&... args) const {
            return fmt::format(fmt_str, std::forward<Args>(args)...);
        }

        /// \brief Format by key (runtime template comes from JSON).
        template <typename... Args>
        std::string textf_key(std::string_view key, Args&&... args) const {
            return fmt::format(text(key), std::forward<Args>(args)...);
        }

        /// \brief vformat by key (when you already have fmt::format_args).
        inline std::string textvf_key(std::string_view key, fmt::format_args args) const {
            return fmt::vformat(text(key), args);
        }

        /// \brief Get "label##id" cached string for ImGui; cache is per key & language.
        const char* label(std::string_view key) const {
            auto it = m_label_cache.find(std::string(key));
            if (it != m_label_cache.end()) return it->second.c_str();

            const std::string& base = text(key);
            std::string combined;
            combined.reserve(base.size() + 2 + key.size());
            combined += base;
            combined += "##";
            combined.append(key.begin(), key.end());

            auto [ins, /*ok*/_] = m_label_cache.emplace(std::string(key), std::move(combined));
            return ins->second.c_str();
        }

        /// \brief Load markdown content for the given doc-key. Fallback to default language.
        /// Resolves: <base>/<lang>/<key>.md -> <base>/<default>/<key>.md
        std::string doc(std::string_view doc_key) const {
            if (auto s = load_md_cached(m_current_lang, doc_key); !s.empty()) return s;
            if (m_current_lang != m_default_lang) {
                if (auto s = load_md_cached(m_default_lang, doc_key); !s.empty()) return s;
            }
            return {};
        }

        /// \brief Clear all runtime caches (labels + markdown). Usually not needed.
        void clear_caches() {
            m_label_cache.clear();
            m_md_cache.clear();
        }

        // ------------------ Pluralization API ------------------

        /// \brief Category suffix for n (via PluralRules).
        /// Returns "one", "few", "many", "other", etc., depending on the active language.
        std::string plural_suffix(long long n) const {
            // PluralRules::category() already has built-ins for "en" and "ru".
            return m_plural_rules ? m_plural_rules->category(m_current_lang, n)
                                  : default_plural_suffix(m_current_lang, n);
        }

        /// \brief Get pluralized text for base key and number.
        /// Tries: "<key>.<suffix>" (cur), "<key>.<suffix>" (default), "<key>" (cur), "<key>" (default).
        const std::string& text_plural(std::string_view base_key, long long n) const {
            const std::string suf = plural_suffix(n);
            if (const auto* s = find_in(*m_current_map, join_dotted(base_key, suf))) return *s;
            if (const auto* s = find_in(m_en_map,      join_dotted(base_key, suf)))   return *s;
            if (const auto* s = find_in(*m_current_map, base_key)) return *s;
            if (const auto* s = find_in(m_en_map,       base_key)) return *s;
            return missing_string();
        }

        template <typename... Args>
        std::string textf_plural(std::string_view base_key, long long n, Args&&... args) const {
            return fmt::format(text_plural(base_key, n), std::forward<Args>(args)...);
        }

    private:

        static std::string default_i18n_base_dir() {
#       if IMGUIX_RESOLVE_PATHS_REL_TO_EXE
            return ImGuiX::Utils::resolveExecPath(IMGUIX_I18N_DIR);
#       else
            return std::string(IMGUIX_I18N_DIR);
#       endif
        }
        // ---------- JSON loading ----------

        StrMap load_language_map(const std::string& lang) const {
            StrMap out;
            const fs::path dir = fs::path(m_base_dir) / lang;
            std::error_code ec;
            if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
                return out; // allowed to be empty
            }

            for (auto& de : fs::directory_iterator(dir, ec)) {
                if (ec) break;
                if (!de.is_regular_file()) continue;
                const auto& p = de.path();
                if (p.extension() != ".json") continue;

                const std::string raw = read_file(p.string());
                if (raw.empty()) continue;

                const std::string clean = ImGuiX::Utils::strip_json_comments(raw, /*with_whitespace*/ false);

                nlohmann::json j;
                try {
                    j = nlohmann::json::parse(clean);
                } catch (...) {
                    // ignore broken files (could log)
                    continue;
                }

                merge_object_for_lang(j, lang, out);
            }
            return out;
        }

        static void merge_object_for_lang(const nlohmann::json& obj,
                                          const std::string& lang,
                                          StrMap& out)
        {
            if (!obj.is_object()) return;

            for (auto it = obj.begin(); it != obj.end(); ++it) {
                const std::string key = it.key();
                const nlohmann::json& val = it.value();

                if (val.is_object()) {
                    // Polyglot: prefer current lang, fallback "en"
                    if (auto jt = val.find(lang); jt != val.end()) {
                        auto s = get_string_or_join_array(*jt);
                        if (!s.empty()) { out[key] = std::move(s); continue; }
                    }
                    if (auto jt = val.find("en"); jt != val.end()) {
                        auto s = get_string_or_join_array(*jt);
                        if (!s.empty()) { out[key] = std::move(s); continue; }
                    }
                } else {
                    // Monolingual
                    auto s = get_string_or_join_array(val);
                    if (!s.empty()) { out[key] = std::move(s); }
                }
            }
        }

        static std::string get_string_or_join_array(const nlohmann::json& j) {
            if (j.is_string()) return j.get<std::string>();
            if (j.is_array()) {
                std::string s;
                for (const auto& e : j) {
                    if (e.is_string()) s += e.get<std::string>();
                }
                return s;
            }
            return {};
        }

        static std::string read_file(const std::string& path) {
            std::ifstream ifs(path, std::ios::binary);
            if (!ifs) return {};
            std::ostringstream oss; oss << ifs.rdbuf();
            return oss.str();
        }

        // ---------- Markdown loading (lazy) ----------

        std::string load_md_cached(const std::string& lang, std::string_view key) const {
            const std::string cache_key = lang + '\n' + std::string(key);
            if (auto it = m_md_cache.find(cache_key); it != m_md_cache.end()) {
                return it->second;
            }

            const fs::path p = fs::path(m_base_dir) / lang / (std::string(key) + ".md");
            const auto s = read_file(p.string());
            if (!s.empty()) {
                m_md_cache.emplace(cache_key, s);
            }
            return s;
        }

        // ---------- Helpers ----------

        static const std::string* find_in(const StrMap& m, std::string_view key) {
            auto it = m.find(std::string(key));
            return (it == m.end()) ? nullptr : &it->second;
        }

        static const std::string& missing_string() {
            static const std::string k = "##null";
            return k;
        }

        static std::string join_dotted(std::string_view base, std::string_view suffix) {
            std::string s;
            s.reserve(base.size() + 1 + suffix.size());
            s.append(base.begin(), base.end());
            s.push_back('.');
            s.append(suffix.begin(), suffix.end());
            return s;
        }

        static std::string default_plural_suffix(const std::string& lang, long long n) {
            // Minimal fallback; PluralRules has richer built-ins.
            if (lang == "ru") {
                long long n10 = n % 10;
                long long n100 = n % 100;
                if (n10 == 1 && n100 != 11) return "one";
                if (n10 >= 2 && n10 <= 4 && !(n100 >= 12 && n100 <= 14)) return "few";
                if (n10 == 0 || (n10 >= 5 && n10 <= 9) || (n100 >= 11 && n100 <= 14)) return "many";
                return "other";
            }
            return (n == 1) ? "one" : "other";
        }

        void try_load_plural_rules_from_default_location() {
            std::error_code ec;
            const fs::path path = fs::u8path(m_base_dir) / IMGUIX_I18N_PLURALS_FILENAME;
            if (fs::exists(path, ec) && m_plural_rules) {
                (void)m_plural_rules->load_from_file(path.u8string()); // ignore failure
            }
        }

    private:
        // config
        std::string m_base_dir;
        std::string m_default_lang;
        std::string m_current_lang;

        // string maps
        StrMap      m_en_map;
        StrMap*     m_current_map{ &m_en_map };

        // caches
        mutable std::unordered_map<std::string, StrMap>        m_lang_cache;   // lang -> map
        mutable std::unordered_map<std::string, std::string>   m_md_cache;     // (lang+'\n'+key) -> content
        mutable std::unordered_map<std::string, std::string>   m_label_cache;  // key -> "text##key"

        // plural rules
        std::unique_ptr<PluralRules> m_plural_rules;
    };

    // -------------------- Optional: tiny plural facade --------------------
    struct Plural {
        static std::string suffix(const LangStore& ls, long long n) {
            return ls.plural_suffix(n);
        }
        static const std::string& text(const LangStore& ls, std::string_view base_key, long long n) {
            return ls.text_plural(base_key, n);
        }
        template <typename... Args>
        static std::string textf(const LangStore& ls, std::string_view base_key, long long n, Args&&... args) {
            return ls.textf_plural(base_key, n, std::forward<Args>(args)...);
        }
    };

} // namespace ImGuiX::Utils::I18N

#endif // _IMGUIX_UTILS_I18N_LANG_STORE_HPP_INCLUDED
