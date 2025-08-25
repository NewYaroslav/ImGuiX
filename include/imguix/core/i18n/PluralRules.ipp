#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace ImGuiX::I18N {

    bool PluralRules::load_from_file(const std::string& path) {
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
            if (!node.contains(u8"cardinal") || !node[u8"cardinal"].is_array()) continue;
            LangRules lr;
            for (const auto& r : node[u8"cardinal"]) {
                if (!r.contains(u8"cat") || !r[u8"cat"].is_string()) continue;
                Rule rule;
                rule.category = r[u8"cat"].get<std::string>();
                rule.cond = r; // keep entire object; evaluator will inspect keys
                lr.rules.emplace_back(std::move(rule));
            }
            if (!lr.rules.empty()) {
                rules_.emplace(lang, std::move(lr));
            }
        }
        return !rules_.empty();
    }

    std::string PluralRules::category(const std::string& lang, long long n) const {
        // Try loaded rules first
        if (auto it = rules_.find(lang); it != rules_.end()) {
            for (const auto& r : it->second.rules) {
                if (eval_rule(r.cond, n)) return r.category;
            }
        }

        // Fallbacks
        if (lang == u8"ru") {
            long long n10 = n % 10;
            long long n100 = n % 100;
            if (n10 == 1 && n100 != 11) return u8"one";
            if (n10 >= 2 && n10 <= 4 && !(n100 >= 12 && n100 <= 14)) return u8"few";
            if (n10 == 0 || (n10 >= 5 && n10 <= 9) || (n100 >= 11 && n100 <= 14)) return u8"many";
            return u8"other";
        }
        // default English-like
        return (n == 1) ? u8"one" : u8"other";
    }

    std::string PluralRules::read_file(const std::string& path) {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) return {};
        std::ostringstream oss; oss << ifs.rdbuf();
        return oss.str();
    }

    bool PluralRules::eval_rule(const nlohmann::json& r, long long n) {
        // order of precedence: structural keys (all/any/not/true), then simple predicates in this node
        if (r.contains(u8"true") && r[u8"true"].is_boolean() && r[u8"true"].get<bool>()) return true;

        if (r.contains(u8"all") && r[u8"all"].is_array()) {
            for (const auto& sub : r[u8"all"]) if (!eval_rule(sub, n)) return false;
            return true;
        }
        if (r.contains(u8"any") && r[u8"any"].is_array()) {
            for (const auto& sub : r[u8"any"]) if (eval_rule(sub, n)) return true;
            return false;
        }
        if (r.contains(u8"not")) {
            return !eval_rule(r[u8"not"], n);
        }

        // Simple numeric predicates at this node
        if (r.contains(u8"eq")) {
            const auto& v = r[u8"eq"];
            if (v.is_number_integer()) return n == v.get<long long>();
        }
        if (r.contains(u8"in") && r[u8"in"].is_array()) {
            for (const auto& x : r[u8"in"]) {
                if (x.is_number_integer() && n == x.get<long long>()) return true;
            }
            return false;
        }
        if (r.contains(u8"range") && r[u8"range"].is_array() && r[u8"range"].size() == 2) {
            const auto a = r[u8"range"][0].get<long long>();
            const auto b = r[u8"range"][1].get<long long>();
            return (n >= a && n <= b);
        }
        if (r.contains(u8"mod_eq") && r[u8"mod_eq"].is_object()) {
            const auto& me = r[u8"mod_eq"];
            if (me.contains(u8"mod") && me.contains(u8"eq")) {
                const auto mod = me[u8"mod"].get<long long>();
                const auto eq  = me[u8"eq"].get<long long>();
                if (mod != 0) return (n % mod) == eq;
            }
        }
        if (r.contains(u8"mod_in_list") && r[u8"mod_in_list"].is_object()) {
            const auto& ml = r[u8"mod_in_list"];
            if (ml.contains(u8"mod") && ml.contains(u8"list") && ml[u8"list"].is_array()) {
                const auto mod = ml[u8"mod"].get<long long>();
                if (mod == 0) return false;
                const long long v = n % mod;
                for (const auto& x : ml[u8"list"]) {
                    if (x.is_number_integer() && v == x.get<long long>()) return true;
                }
                return false;
            }
        }
        if (r.contains(u8"mod_in_range") && r[u8"mod_in_range"].is_object()) {
            const auto& mr = r[u8"mod_in_range"];
            if (mr.contains(u8"mod") && mr.contains(u8"range") &&
                mr[u8"range"].is_array() && mr[u8"range"].size() == 2) {
                const auto mod = mr[u8"mod"].get<long long>();
                if (mod == 0) return false;
                const long long v = n % mod;
                const auto a = mr[u8"range"][0].get<long long>();
                const auto b = mr[u8"range"][1].get<long long>();
                return (v >= a && v <= b);
            }
        }

        // If no predicate matched in this node, consider it non-matching.
        return false;
    }

} // namespace ImGuiX::I18N

