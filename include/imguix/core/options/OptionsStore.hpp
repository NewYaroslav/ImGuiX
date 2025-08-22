#pragma once
#ifndef _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED
#define _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED

/// \file OptionsStore.hpp
/// \brief 

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "OptionsStoreViewCRTP.hpp"
#include "OptionsStoreControlCRTP.hpp"

namespace ImGuiX {

    /// \class OptionsStore
    /// \brief Durable JSON-backed key-value options storage.
    /// \note Thread-safe. Call update() periodically to perform debounced saves.
    class OptionsStore :
        private OptionsStoreViewCRTP<OptionsStore>,
        private OptionsStoreControlCRTP<OptionsStore> {
    public:
        using View = OptionsStoreViewCRTP<OptionsStore>;
        using Control = OptionsStoreControlCRTP<OptionsStore>;

        /// \brief Construct store.
        /// \param path JSON file path.
        /// \param save_delay_sec Debounce window for saving (seconds).
        explicit OptionsStore(std::string path, double save_delay_sec = 0.5);
        ~OptionsStore();

        /// \brief Load from disk (best-effort). Keeps existing values on failure.
        void load() noexcept;

        /// \brief Force immediate save (best-effort, atomic write).
        void saveNow() noexcept;

        /// \brief Tick method. Saves if there were changes and debounce elapsed.
        void update() noexcept;

        // --- meta ---
        /// \brief Set store version.
        /// \param ver New version.
        void setVersion(std::int32_t ver) noexcept;

        /// \brief Get store version.
        /// \return Version.
        std::int32_t version() const noexcept;

        // --- wrappers for CRTP ---
        /// \brief Check whether key exists.
        /// \param key Option key.
        /// \return True if key exists.
        bool has(const std::string& key) const noexcept;

        /// \brief Erase option by key.
        /// \param key Option key.
        /// \return True if key was erased.
        bool erase(const std::string& key) noexcept;

        /// \brief Get list of keys.
        /// \return Vector of keys.
        std::vector<std::string> keys() const;

        /// \brief Set boolean option.
        /// \param key Option key.
        /// \param v New value.
        void setBool(const std::string& key, bool v) noexcept;

        /// \brief Set 32-bit integer option.
        /// \param key Option key.
        /// \param v New value.
        void setI32(const std::string& key, std::int32_t v) noexcept;

        /// \brief Set 64-bit integer option.
        /// \param key Option key.
        /// \param v New value.
        void setI64(const std::string& key, std::int64_t v) noexcept;

        /// \brief Set float option.
        /// \param key Option key.
        /// \param v New value.
        void setF32(const std::string& key, float v) noexcept;

        /// \brief Set double option.
        /// \param key Option key.
        /// \param v New value.
        void setF64(const std::string& key, double v) noexcept;

        /// \brief Set string option.
        /// \param key Option key.
        /// \param v New value.
        void setStr(const std::string& key, const std::string& v) noexcept;

        /// \brief Set string vector option.
        /// \param key Option key.
        /// \param v New value.
        void setStrVec(
                const std::string& key,
                const std::vector<std::string>& v) noexcept;

        /// \brief Get boolean option.
        /// \param key Option key.
        /// \return Option value.
        bool getBool(const std::string& key) const;

        /// \brief Get 32-bit integer option.
        /// \param key Option key.
        /// \return Option value.
        std::int32_t getI32(const std::string& key) const;

        /// \brief Get 64-bit integer option.
        /// \param key Option key.
        /// \return Option value.
        std::int64_t getI64(const std::string& key) const;

        /// \brief Get float option.
        /// \param key Option key.
        /// \return Option value.
        float getF32(const std::string& key) const;

        /// \brief Get double option.
        /// \param key Option key.
        /// \return Option value.
        double getF64(const std::string& key) const;

        /// \brief Get string option.
        /// \param key Option key.
        /// \return Option value.
        std::string getStr(const std::string& key) const;

        /// \brief Get string vector option.
        /// \param key Option key.
        /// \return Option value.
        std::vector<std::string> getStrVec(const std::string& key) const;

        /// \brief Get boolean option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        bool getBoolOr(const std::string& key, bool def) const noexcept;

        /// \brief Get 32-bit integer option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        std::int32_t getI32Or(const std::string& key, std::int32_t def) const noexcept;

        /// \brief Get 64-bit integer option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        std::int64_t getI64Or(const std::string& key, std::int64_t def) const noexcept;

        /// \brief Get float option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        float getF32Or(const std::string& key, float def) const noexcept;

        /// \brief Get double option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        double getF64Or(const std::string& key, double def) const noexcept;

        /// \brief Get string option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        std::string getStrOr(const std::string& key, std::string def) const noexcept;

        /// \brief Get string vector option or default.
        /// \param key Option key.
        /// \param def Default value.
        /// \return Option value or default.
        std::vector<std::string> getStrVecOr(
                const std::string& key,
                std::vector<std::string> def) const noexcept;

        /// \brief Get read-only view.
        /// \return View interface.
        View& view() noexcept { return *this; }

        /// \brief Get read-only view.
        /// \return View interface.
        const View& view() const noexcept { return *this; }

        /// \brief Get mutable interface without load/save.
        /// \return Control interface.
        Control& control() noexcept { return *this; }

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        OptionsStore(const OptionsStore&) = delete;
        OptionsStore& operator=(const OptionsStore&) = delete;
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "OptionsStore.ipp"
#endif

#endif // _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED
