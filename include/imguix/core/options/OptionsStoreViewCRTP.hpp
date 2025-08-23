#pragma once
#ifndef _IMGUIX_CORE_OPTIONS_STORE_VIEW_CRTP_HPP_INCLUDED
#define _IMGUIX_CORE_OPTIONS_STORE_VIEW_CRTP_HPP_INCLUDED

/// \file OptionsStoreViewCRTP.hpp
/// \brief Read-only facade for \c OptionsStore access.

#include <string>
#include <vector>
#include <cstdint>

namespace ImGuiX {

    /// \brief Read-only facade for OptionsStore access.
    /// \tparam Impl Concrete OptionsStore implementation.
    template<class Impl>
    struct OptionsStoreViewCRTP {
        /// \copydoc OptionsStore::has
        bool has(const std::string& key) const noexcept {
            return static_cast<const Impl*>(this)->has(key);
        }

        /// \copydoc OptionsStore::keys
        std::vector<std::string> keys() const {
            return static_cast<const Impl*>(this)->keys();
        }

        /// \copydoc OptionsStore::getBool
        bool getBool(const std::string& key) const {
            return static_cast<const Impl*>(this)->getBool(key);
        }

        /// \copydoc OptionsStore::getI32
        std::int32_t getI32(const std::string& key) const {
            return static_cast<const Impl*>(this)->getI32(key);
        }

        /// \copydoc OptionsStore::getI64
        std::int64_t getI64(const std::string& key) const {
            return static_cast<const Impl*>(this)->getI64(key);
        }

        /// \copydoc OptionsStore::getF32
        float getF32(const std::string& key) const {
            return static_cast<const Impl*>(this)->getF32(key);
        }

        /// \copydoc OptionsStore::getF64
        double getF64(const std::string& key) const {
            return static_cast<const Impl*>(this)->getF64(key);
        }

        /// \copydoc OptionsStore::getStr
        std::string getStr(const std::string& key) const {
            return static_cast<const Impl*>(this)->getStr(key);
        }

        /// \copydoc OptionsStore::getStrVec
        std::vector<std::string> getStrVec(const std::string& key) const {
            return static_cast<const Impl*>(this)->getStrVec(key);
        }

        /// \copydoc OptionsStore::getBoolOr
        bool getBoolOr(const std::string& key, bool def) const noexcept {
            return static_cast<const Impl*>(this)->getBoolOr(key, def);
        }

        /// \copydoc OptionsStore::getI32Or
        std::int32_t getI32Or(const std::string& key, std::int32_t def) const noexcept {
            return static_cast<const Impl*>(this)->getI32Or(key, def);
        }

        /// \copydoc OptionsStore::getI64Or
        std::int64_t getI64Or(const std::string& key, std::int64_t def) const noexcept {
            return static_cast<const Impl*>(this)->getI64Or(key, def);
        }

        /// \copydoc OptionsStore::getF32Or
        float getF32Or(const std::string& key, float def) const noexcept {
            return static_cast<const Impl*>(this)->getF32Or(key, def);
        }

        /// \copydoc OptionsStore::getF64Or
        double getF64Or(const std::string& key, double def) const noexcept {
            return static_cast<const Impl*>(this)->getF64Or(key, def);
        }

        /// \copydoc OptionsStore::getStrOr
        std::string getStrOr(const std::string& key, std::string def) const noexcept {
            return static_cast<const Impl*>(this)->getStrOr(key, std::move(def));
        }

        /// \copydoc OptionsStore::getStrVecOr
        std::vector<std::string> getStrVecOr(
                const std::string& key,
                std::vector<std::string> def) const noexcept {
            return static_cast<const Impl*>(this)->getStrVecOr(key, std::move(def));
        }

        /// \copydoc OptionsStore::version
        std::int32_t version() const noexcept {
            return static_cast<const Impl*>(this)->version();
        }
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_OPTIONS_STORE_VIEW_CRTP_HPP_INCLUDED
