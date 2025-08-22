#pragma once
#ifndef _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED
#define _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED

/// \file OptionsStoreControlCRTP.hpp
/// \brief 

#include <string>
#include <vector>
#include <cstdint>

namespace ImGuiX {

    /// \brief Mutable facade for OptionsStore access.
    /// \tparam Impl Concrete OptionsStore implementation.
    template<class Impl>
    struct OptionsStoreControlCRTP {
        bool has(const std::string& key) const noexcept {
            return static_cast<const Impl*>(this)->has(key);
        }

        bool erase(const std::string& key) {
            return static_cast<Impl*>(this)->erase(key);
        }

        std::vector<std::string> keys() const {
            return static_cast<const Impl*>(this)->keys();
        }

        void setBool(const std::string& key, bool v) {
            static_cast<Impl*>(this)->setBool(key, v);
        }

        void setI32(const std::string& key, std::int32_t v) {
            static_cast<Impl*>(this)->setI32(key, v);
        }

        void setI64(const std::string& key, std::int64_t v) {
            static_cast<Impl*>(this)->setI64(key, v);
        }

        void setF32(const std::string& key, float v) {
            static_cast<Impl*>(this)->setF32(key, v);
        }

        void setF64(const std::string& key, double v) {
            static_cast<Impl*>(this)->setF64(key, v);
        }

        void setStr(const std::string& key, const std::string& v) {
            static_cast<Impl*>(this)->setStr(key, v);
        }

        void setStrVec(
                const std::string& key,
                const std::vector<std::string>& v) {
            static_cast<Impl*>(this)->setStrVec(key, v);
        }

        bool getBool(const std::string& key) const {
            return static_cast<const Impl*>(this)->getBool(key);
        }

        std::int32_t getI32(const std::string& key) const {
            return static_cast<const Impl*>(this)->getI32(key);
        }

        std::int64_t getI64(const std::string& key) const {
            return static_cast<const Impl*>(this)->getI64(key);
        }

        float getF32(const std::string& key) const {
            return static_cast<const Impl*>(this)->getF32(key);
        }

        double getF64(const std::string& key) const {
            return static_cast<const Impl*>(this)->getF64(key);
        }

        std::string getStr(const std::string& key) const {
            return static_cast<const Impl*>(this)->getStr(key);
        }

        std::vector<std::string> getStrVec(const std::string& key) const {
            return static_cast<const Impl*>(this)->getStrVec(key);
        }

        bool getBoolOr(const std::string& key, bool def) const noexcept {
            return static_cast<const Impl*>(this)->getBoolOr(key, def);
        }

        std::int32_t getI32Or(const std::string& key, std::int32_t def) const noexcept {
            return static_cast<const Impl*>(this)->getI32Or(key, def);
        }

        std::int64_t getI64Or(const std::string& key, std::int64_t def) const noexcept {
            return static_cast<const Impl*>(this)->getI64Or(key, def);
        }

        float getF32Or(const std::string& key, float def) const noexcept {
            return static_cast<const Impl*>(this)->getF32Or(key, def);
        }

        double getF64Or(const std::string& key, double def) const noexcept {
            return static_cast<const Impl*>(this)->getF64Or(key, def);
        }

        std::string getStrOr(const std::string& key, std::string def) const noexcept {
            return static_cast<const Impl*>(this)->getStrOr(key, std::move(def));
        }

        std::vector<std::string> getStrVecOr(
                const std::string& key,
                std::vector<std::string> def) const noexcept {
            return static_cast<const Impl*>(this)->getStrVecOr(key, std::move(def));
        }

        std::int32_t version() const noexcept {
            return static_cast<const Impl*>(this)->version();
        }
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_OPTIONS_STORE_CONTROL_CRTP_HPP_INCLUDED
