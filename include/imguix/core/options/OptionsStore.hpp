#pragma once
#ifndef _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED
#define _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "OptionsStoreViewCRTP.hpp"
#include "OptionsStoreControlCRTP.hpp"

namespace ImGuiX {

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
        void setVersion(std::int32_t ver) noexcept;
        std::int32_t version() const noexcept;

        // --- wrappers for CRTP ---
        bool has(const std::string& key) const noexcept;
        bool erase(const std::string& key) noexcept;
        std::vector<std::string> keys() const;

        void setBool(const std::string& key, bool v) noexcept;
        void setI32(const std::string& key, std::int32_t v) noexcept;
        void setI64(const std::string& key, std::int64_t v) noexcept;
        void setF32(const std::string& key, float v) noexcept;
        void setF64(const std::string& key, double v) noexcept;
        void setStr(const std::string& key, const std::string& v) noexcept;
        void setStrVec(
                const std::string& key,
                const std::vector<std::string>& v) noexcept;

        bool getBool(const std::string& key) const;
        std::int32_t getI32(const std::string& key) const;
        std::int64_t getI64(const std::string& key) const;
        float getF32(const std::string& key) const;
        double getF64(const std::string& key) const;
        std::string getStr(const std::string& key) const;
        std::vector<std::string> getStrVec(const std::string& key) const;

        bool getBoolOr(const std::string& key, bool def) const noexcept;
        std::int32_t getI32Or(const std::string& key, std::int32_t def) const noexcept;
        std::int64_t getI64Or(const std::string& key, std::int64_t def) const noexcept;
        float getF32Or(const std::string& key, float def) const noexcept;
        double getF64Or(const std::string& key, double def) const noexcept;
        std::string getStrOr(const std::string& key, std::string def) const noexcept;
        std::vector<std::string> getStrVecOr(
                const std::string& key,
                std::vector<std::string> def) const noexcept;

        /// \brief Read-only view.
        View& view() noexcept { return *this; }
        /// \brief Read-only view (const).
        const View& view() const noexcept { return *this; }
        /// \brief Mutable interface without load/save.
        Control& control() noexcept { return *this; }

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        OptionsStore(const OptionsStore&) = delete;
        OptionsStore& operator=(const OptionsStore&) = delete;
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#include "OptionsStore.ipp"
#endif

#endif // _IMGUIX_CORE_OPTIONS_STORE_HPP_INCLUDED
