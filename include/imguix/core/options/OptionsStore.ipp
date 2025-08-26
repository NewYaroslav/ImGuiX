#include <nlohmann/json.hpp>
#include <chrono>
#include <fstream>
#include <optional>
#include <cstdio>
#include <mutex>

#include <imguix/config/paths.hpp>
#include <imguix/utils/path_utils.hpp>

namespace ImGuiX {

    using json = nlohmann::json;
    using Clock = std::chrono::steady_clock;

    struct OptionsStore::Impl {
        std::string m_path;
        std::string m_tmp_path;
        double m_save_delay{0.5};

        mutable std::mutex m_mutex;
        json m_root = json::object();
        bool m_dirty{false};
        Clock::time_point m_last_touch{Clock::now()};

        void touchLocked() {
            m_dirty = true;
            m_last_touch = Clock::now();
        }

        static json& ensureMeta(json& root) {
            if (!root.is_object()) root = json::object();
            auto it = root.find(u8"__meta");
            if (it == root.end() || !it->is_object()) {
                root[u8"__meta"] = json::object();
            }
            return root[u8"__meta"];
        }

        static const json* meta(const json& root) {
            if (!root.is_object()) return nullptr;
            auto it = root.find(u8"__meta");
            if (it == root.end() || !it->is_object()) return nullptr;
            return &(*it);
        }

        static bool isApproxInteger(double d) {
            return std::llround(d) == static_cast<long long>(d);
        }

        void saveLockedNoexcept() {
            try {
                {
                    std::ofstream tf(
                            m_tmp_path,
                            std::ios::binary | std::ios::trunc);
                    if (!tf.good()) throw std::runtime_error(u8"tmp open failed");
                    tf << m_root.dump(4);
                    tf.flush();
                }
#ifdef _WIN32
                MoveFileExA(m_tmp_path.c_str(), m_path.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
#else
                std::rename(m_tmp_path.c_str(), m_path.c_str());
#endif
            } catch (...) {
                std::remove(m_tmp_path.c_str());
            }
        }

        static std::optional<bool> asBool(const json& v) {
            if (v.is_boolean()) return v.get<bool>();
            if (v.is_number_integer()) return v.get<long long>() != 0;
            if (v.is_string()) {
                auto s = v.get<std::string>();
                if (s == u8"true" || s == u8"1") return true;
                if (s == u8"false" || s == u8"0") return false;
            }
            return std::nullopt;
        }

        template<class Int>
        static std::optional<Int> asInt(const json& v) {
            static_assert(std::is_same<Int, int32_t>::value ||
                          std::is_same<Int, int64_t>::value,
                          u8"int type");
            if (v.is_number_integer()) {
                long long x = v.get<long long>();
                if constexpr (std::is_same<Int, int32_t>::value) {
                    if (x < INT32_MIN || x > INT32_MAX) return std::nullopt;
                    return static_cast<int32_t>(x);
                } else {
                    return static_cast<int64_t>(x);
                }
            }
            if (v.is_number_float()) {
                double d = v.get<double>();
                if (!isApproxInteger(d)) return std::nullopt;
                long long x = static_cast<long long>(d);
                if constexpr (std::is_same<Int, int32_t>::value) {
                    if (x < INT32_MIN || x > INT32_MAX) return std::nullopt;
                    return static_cast<int32_t>(x);
                } else {
                    return static_cast<int64_t>(x);
                }
            }
            if (v.is_string()) {
                try {
                    long long x = std::stoll(v.get<std::string>());
                    if constexpr (std::is_same<Int, int32_t>::value) {
                        if (x < INT32_MIN || x > INT32_MAX) return std::nullopt;
                        return static_cast<int32_t>(x);
                    } else {
                        return static_cast<int64_t>(x);
                    }
                } catch (...) {}
            }
            return std::nullopt;
        }

        template<class F>
        static std::optional<F> asFloat(const json& v) {
            static_assert(std::is_same<F, float>::value ||
                          std::is_same<F, double>::value,
                          u8"float type");
            if (v.is_number()) return static_cast<F>(v.get<double>());
            if (v.is_string()) {
                try {
                    double d = std::stod(v.get<std::string>());
                    return static_cast<F>(d);
                } catch (...) {}
            }
            return std::nullopt;
        }

        static std::optional<std::string> asString(const json& v) {
            if (v.is_string()) return v.get<std::string>();
            if (v.is_number_integer()) return std::to_string(v.get<long long>());
            if (v.is_number_float()) return std::to_string(v.get<double>());
            if (v.is_boolean()) return v.get<bool>() ? u8"true" : u8"false";
            return std::nullopt;
        }

        static std::optional<std::vector<std::string>> asStrVec(const json& v) {
            if (!v.is_array()) return std::nullopt;
            std::vector<std::string> out;
            out.reserve(v.size());
            for (auto& e : v) {
                if (auto s = asString(e)) out.push_back(std::move(*s));
                else return std::nullopt;
            }
            return out;
        }
    };

    inline OptionsStore::OptionsStore(std::string path, double save_delay_sec)
        : m_impl(std::make_unique<Impl>()) {
        m_impl->m_path = std::move(path);
        m_impl->m_tmp_path = m_impl->m_path + u8".tmp";
        m_impl->m_save_delay = save_delay_sec;
        load();
    }
    
    inline OptionsStore::OptionsStore() 
        : m_impl(std::make_unique<Impl>()) {
        const std::string base_dir(IMGUIX_CONFIG_DIR);
        const auto base_abs = ImGuiX::Utils::resolveExecPath(base_dir);
        std::string path = ImGuiX::Utils::joinPaths(base_abs, u8"options.json");
        m_impl->m_path = std::move(path);
        m_impl->m_tmp_path = m_impl->m_path + u8".tmp";
        m_impl->m_save_delay = 0.5;
        load();
    }

    inline OptionsStore::~OptionsStore() = default;

    inline void OptionsStore::load() noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        std::ifstream f(m_impl->m_path);
        if (!f.good()) return;
        try {
            json j2;
            f >> j2;
            if (!j2.is_object()) return;
            m_impl->m_root = std::move(j2);
        } catch (...) {}
    }

    inline void OptionsStore::saveNow() noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_dirty = false;
        m_impl->saveLockedNoexcept();
    }

    inline void OptionsStore::update() noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        if (!m_impl->m_dirty) return;
        double elapsed = std::chrono::duration<double>(
                Clock::now() - m_impl->m_last_touch).count();
        if (elapsed >= m_impl->m_save_delay) {
            m_impl->m_dirty = false;
            m_impl->saveLockedNoexcept();
        }
    }

    inline bool OptionsStore::has(const std::string& key) const noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        return m_impl->m_root.contains(key);
    }

    inline bool OptionsStore::erase(const std::string& key) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end()) return false;
        m_impl->m_root.erase(it);
        m_impl->touchLocked();
        return true;
    }

    inline std::vector<std::string> OptionsStore::keys() const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        std::vector<std::string> out;
        out.reserve(m_impl->m_root.size());
        for (auto it = m_impl->m_root.begin(); it != m_impl->m_root.end(); ++it) {
            if (it.key() == u8"__meta") continue;
            out.push_back(it.key());
        }
        return out;
    }

    inline void OptionsStore::setBool(const std::string& key, bool v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = v;
        m_impl->touchLocked();
    }

    inline void OptionsStore::setI32(const std::string& key, std::int32_t v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = static_cast<long long>(v);
        m_impl->touchLocked();
    }

    inline void OptionsStore::setI64(const std::string& key, std::int64_t v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = static_cast<long long>(v);
        m_impl->touchLocked();
    }

    inline void OptionsStore::setF32(const std::string& key, float v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = static_cast<double>(v);
        m_impl->touchLocked();
    }

    inline void OptionsStore::setF64(const std::string& key, double v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = v;
        m_impl->touchLocked();
    }

    inline void OptionsStore::setStr(
            const std::string& key,
            const std::string& v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = v;
        m_impl->touchLocked();
    }

    inline void OptionsStore::setStrVec(
            const std::string& key,
            const std::vector<std::string>& v) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        m_impl->m_root[key] = v;
        m_impl->touchLocked();
    }

    inline bool OptionsStore::getBool(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asBool(*it)) return *x;
        throw std::bad_cast();
    }

    inline std::int32_t OptionsStore::getI32(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asInt<int32_t>(*it)) return *x;
        throw std::bad_cast();
    }

    inline std::int64_t OptionsStore::getI64(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asInt<int64_t>(*it)) return *x;
        throw std::bad_cast();
    }

    inline float OptionsStore::getF32(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asFloat<float>(*it)) return *x;
        throw std::bad_cast();
    }

    inline double OptionsStore::getF64(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asFloat<double>(*it)) return *x;
        throw std::bad_cast();
    }

    inline std::string OptionsStore::getStr(const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asString(*it)) return *x;
        throw std::bad_cast();
    }

    inline std::vector<std::string> OptionsStore::getStrVec(
            const std::string& key) const {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto it = m_impl->m_root.find(key);
        if (it == m_impl->m_root.end())
            throw std::out_of_range(u8"key not found: " + key);
        if (auto x = Impl::asStrVec(*it)) return *x;
        throw std::bad_cast();
    }

    inline bool OptionsStore::getBoolOr(
            const std::string& key,
            bool def) const noexcept {
        try {
            return getBool(key);
        } catch (...) {
            return def;
        }
    }

    inline std::int32_t OptionsStore::getI32Or(
            const std::string& key,
            std::int32_t def) const noexcept {
        try {
            return getI32(key);
        } catch (...) {
            return def;
        }
    }

    inline std::int64_t OptionsStore::getI64Or(
            const std::string& key,
            std::int64_t def) const noexcept {
        try {
            return getI64(key);
        } catch (...) {
            return def;
        }
    }

    inline float OptionsStore::getF32Or(
            const std::string& key,
            float def) const noexcept {
        try {
            return getF32(key);
        } catch (...) {
            return def;
        }
    }

    inline double OptionsStore::getF64Or(
            const std::string& key,
            double def) const noexcept {
        try {
            return getF64(key);
        } catch (...) {
            return def;
        }
    }

    inline std::string OptionsStore::getStrOr(
            const std::string& key,
            std::string def) const noexcept {
        try {
            return getStr(key);
        } catch (...) {
            return def;
        }
    }

    inline std::vector<std::string> OptionsStore::getStrVecOr(
            const std::string& key,
            std::vector<std::string> def) const noexcept {
        try {
            return getStrVec(key);
        } catch (...) {
            return def;
        }
    }

    inline void OptionsStore::setVersion(std::int32_t ver) noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto& m = Impl::ensureMeta(m_impl->m_root);
        m[u8"version"] = static_cast<long long>(ver);
        m_impl->touchLocked();
    }

    inline std::int32_t OptionsStore::version() const noexcept {
        std::lock_guard<std::mutex> lk(m_impl->m_mutex);
        auto* m = Impl::meta(m_impl->m_root);
        if (!m) return 0;
        if (m->contains(u8"version")) {
            const json& v = (*m)[u8"version"];
            if (v.is_number_integer()) {
                long long x = v.get<long long>();
                if (x < INT32_MIN || x > INT32_MAX) return 0;
                return static_cast<int32_t>(x);
            }
        }
        return 0;
    }

} // namespace ImGuiX
