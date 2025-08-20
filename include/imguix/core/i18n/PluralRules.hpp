#pragma once
#ifndef _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED
#define _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED

/// \file PluralRules.hpp
/// \brief Pluggable plural rules: built-in fallback (en/ru) + JSON file loader.
/// JSON format (see docs): per-language "cardinal" array of rules with conditions.

namespace ImGuiX::Utils::I18N {

    namespace fs = std::filesystem;

    class PluralRules {
    public:
        // Load rules from JSON file. Returns true on success.
        bool load_from_file(const std::string& path) {
            std::error_code ec;
            if (!fs::exists(path, ec)) return false;

            const auto content = read_file(path);
            if (content.empty()) return false;

            nlohmann::json j;
            try {
                j = nlohmann::json::parse(content);
            } catch (...) {
                return false;
            }

            rules_.clear();
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string lang = it.key();
                const auto& node = it.value();
                if (!node.contains("cardinal") || !node["cardinal"].is_array()) continue;
                LangRules lr;
                for (const auto& r : node["cardinal"]) {
                    if (!r.contains("cat") || !r["cat"].is_string()) continue;
                    Rule rule;
                    rule.category = r["cat"].get<std::string>();
                    rule.cond = r; // keep entire object; evaluator will inspect keys
                    lr.rules.emplace_back(std::move(rule));
                }
                if (!lr.rules.empty()) {
                    rules_.emplace(lang, std::move(lr));
                }
            }
            return !rules_.empty();
        }

        // Return category for number n in language 'lang'.
        // Built-in fallback: 'en' (one/other) and 'ru' (one/few/many/other).
        std::string category(const std::string& lang, long long n) const {
            // Try loaded rules first
            if (auto it = rules_.find(lang); it != rules_.end()) {
                for (const auto& r : it->second.rules) {
                    if (eval_rule(r.cond, n)) return r.category;
                }
            }

            // Fallbacks
            if (lang == "ru") {
                long long n10 = n % 10;
                long long n100 = n % 100;
                if (n10 == 1 && n100 != 11) return "one";
                if (n10 >= 2 && n10 <= 4 && !(n100 >= 12 && n100 <= 14)) return "few";
                if (n10 == 0 || (n10 >= 5 && n10 <= 9) || (n100 >= 11 && n100 <= 14)) return "many";
                return "other";
            }
            // default English-like
            return (n == 1) ? "one" : "other";
        }

    private:
        struct Rule {
            std::string category;
            nlohmann::json cond; // condition object
        };
        struct LangRules {
            std::vector<Rule> rules;
        };
        std::unordered_map<std::string, LangRules> rules_;

        static std::string read_file(const std::string& path) {
            std::ifstream ifs(path, std::ios::binary);
            if (!ifs) return {};
            std::ostringstream oss; oss << ifs.rdbuf();
            return oss.str();
        }

        // --- evaluator ---

        static bool eval_rule(const nlohmann::json& r, long long n) {
            // order of precedence: structural keys (all/any/not/true), then simple predicates in this node
            if (r.contains("true") && r["true"].is_boolean() && r["true"].get<bool>()) return true;

            if (r.contains("all") && r["all"].is_array()) {
                for (const auto& sub : r["all"]) if (!eval_rule(sub, n)) return false;
                return true;
            }
            if (r.contains("any") && r["any"].is_array()) {
                for (const auto& sub : r["any"]) if (eval_rule(sub, n)) return true;
                return false;
            }
            if (r.contains("not")) {
                return !eval_rule(r["not"], n);
            }

            // Simple numeric predicates at this node
            if (r.contains("eq")) {
                const auto& v = r["eq"];
                if (v.is_number_integer()) return n == v.get<long long>();
            }
            if (r.contains("in") && r["in"].is_array()) {
                for (const auto& x : r["in"]) {
                    if (x.is_number_integer() && n == x.get<long long>()) return true;
                }
                return false;
            }
            if (r.contains("range") && r["range"].is_array() && r["range"].size() == 2) {
                const auto a = r["range"][0].get<long long>();
                const auto b = r["range"][1].get<long long>();
                return (n >= a && n <= b);
            }
            if (r.contains("mod_eq") && r["mod_eq"].is_object()) {
                const auto& me = r["mod_eq"];
                if (me.contains("mod") && me.contains("eq")) {
                    const auto mod = me["mod"].get<long long>();
                    const auto eq  = me["eq"].get<long long>();
                    if (mod != 0) return (n % mod) == eq;
                }
            }
            if (r.contains("mod_in_list") && r["mod_in_list"].is_object()) {
                const auto& ml = r["mod_in_list"];
                if (ml.contains("mod") && ml.contains("list") && ml["list"].is_array()) {
                    const auto mod = ml["mod"].get<long long>();
                    if (mod == 0) return false;
                    const long long v = n % mod;
                    for (const auto& x : ml["list"]) {
                        if (x.is_number_integer() && v == x.get<long long>()) return true;
                    }
                    return false;
                }
            }
            if (r.contains("mod_in_range") && r["mod_in_range"].is_object()) {
                const auto& mr = r["mod_in_range"];
                if (mr.contains("mod") && mr.contains("range") &&
                    mr["range"].is_array() && mr["range"].size() == 2) {
                    const auto mod = mr["mod"].get<long long>();
                    if (mod == 0) return false;
                    const long long v = n % mod;
                    const auto a = mr["range"][0].get<long long>();
                    const auto b = mr["range"][1].get<long long>();
                    return (v >= a && v <= b);
                }
            }

            // If no predicate matched in this node, consider it non-matching.
            return false;
        }
    };

} // namespace ImGuiX::Utils::I18N

#endif // _IMGUIX_UTILS_I18N_PLURAL_RULES_HPP_INCLUDED
