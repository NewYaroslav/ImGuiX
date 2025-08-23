#pragma once
#ifndef _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED
#define _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED

/// \file OptionsStoreControlCRTP.hpp
/// \brief Mutable facade providing full access to \c OptionsStore.

#include <string>
#include <vector>
#include <cstdint>

namespace ImGuiX {

    /// \brief Mutable facade for OptionsStore access.
    /// \tparam Impl Concrete OptionsStore implementation.
    template<class Impl>
    struct OptionsStoreControlCRTP {
        /// \copydoc OptionsStore::has
        bool has(const std::string& key) const noexcept {
            return static_cast<const Impl*>(this)->has(key);
        }

        /// \copydoc OptionsStore::erase
        bool erase(const std::string& key) {
            return static_cast<Impl*>(this)->erase(key);
        }

        /// \copydoc OptionsStore::keys
        std::vector<std::string> keys() const {
            return static_cast<const Impl*>(this)->keys();
        }

        /// \copydoc OptionsStore::setBool
        void setBool(const std::string& key, bool v) {
            static_cast<Impl*>(this)->setBool(key, v);
        }

        /// \copydoc OptionsStore::setI32
        void setI32(const std::string& key, std::int32_t v) {
            static_cast<Impl*>(this)->setI32(key, v);
        }

        /// \copydoc OptionsStore::setI64
        void setI64(const std::string& key, std::int64_t v) {
            static_cast<Impl*>(this)->setI64(key, v);
        }

        /// \copydoc OptionsStore::setF32
        void setF32(const std::string& key, float v) {
            static_cast<Impl*>(this)->setF32(key, v);
        }

        /// \copydoc OptionsStore::setF64
        void setF64(const std::string& key, double v) {
            static_cast<Impl*>(this)->setF64(key, v);
        }

        /// \copydoc OptionsStore::setStr
        void setStr(const std::string& key, const std::string& v) {
            static_cast<Impl*>(this)->setStr(key, v);
        }

        /// \copydoc OptionsStore::setStrVec
        void setStrVec(
                const std::string& key,
                const std::vector<std::string>& v) {
            static_cast<Impl*>(this)->setStrVec(key, v);
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

#endif // _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED
