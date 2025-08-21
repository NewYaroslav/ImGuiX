#pragma once
#ifndef _IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED
#define _IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED

/// \file path_utils.hpp
/// \brief Path utilities for resolving executable-relative paths and file manipulations.

#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

#if defined(_WIN32)
#   include <Windows.h>
#   include <codecvt>
#   include <locale>
#elif defined(__APPLE__)
#   include <mach-o/dyld.h>
#   include <limits.h>
#   include <unistd.h>
#else
#   include <limits.h>
#   include <unistd.h>
#endif

namespace ImGuiX::Utils {
    namespace fs = std::filesystem;
    namespace detail {
        template<typename T>
        inline constexpr bool dependent_false_v = false;
    }

    /// \brief Return full path to the current executable.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \return Executable path.
    /// \throws std::runtime_error on failure.
    template<typename Dummy = void>
    std::string getExecPath() {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "getExecPath is not supported on Emscripten");
        return {};
#elif defined(_WIN32)
        std::vector<wchar_t> buffer(MAX_PATH);
        HMODULE hModule = GetModuleHandle(nullptr);

        DWORD size = GetModuleFileNameW(hModule, buffer.data(), static_cast<DWORD>(buffer.size()));
        while (size == buffer.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer.resize(buffer.size() * 2);
            size = GetModuleFileNameW(hModule, buffer.data(), static_cast<DWORD>(buffer.size()));
        }

        if (size == 0)
            throw std::runtime_error("Failed to get executable path.");

        std::wstring exe_path(buffer.begin(), buffer.begin() + size);
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(exe_path);
#elif defined(__APPLE__)
        std::vector<char> buffer(PATH_MAX);
        uint32_t size = buffer.size();
        if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
            buffer.resize(size);
            if (_NSGetExecutablePath(buffer.data(), &size) != 0)
                throw std::runtime_error("Failed to get executable path.");
        }
        return std::string(buffer.data());
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count == -1)
            throw std::runtime_error("Failed to get executable path.");

        return std::string(result, count);
#endif
    }

    /// \brief Return directory of the executable.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \return Directory path.
    /// \throws std::runtime_error if getExecPath fails.
    template<typename Dummy = void>
    std::string getExecDir() {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "getExecDir is not supported on Emscripten");
        return {};
#else
        fs::path path = fs::u8path(getExecPath());
        return path.parent_path().u8string();
#endif
    }

    /// \brief Resolve relative path to absolute using executable directory.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \param relative_path Relative path from executable directory.
    /// \return Absolute path string.
    template<typename Dummy = void>
    std::string resolveExecPath(const std::string& relative_path) {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "resolveExecPath is not supported on Emscripten");
        return relative_path;
#else
        return (fs::u8path(getExecDir()) / fs::u8path(relative_path)).u8string();
#endif
    }

    /// \brief Extract filename from full path.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \param full_path Full path to file.
    /// \return Filename component.
    template<typename Dummy = void>
    std::string getFileName(const std::string& full_path) {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "getFileName is not supported on Emscripten");
        return full_path;
#else
        return fs::u8path(full_path).filename().u8string();
#endif
    }

    /// \brief Compute relative path from base to target.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \param file_path Target file path.
    /// \param base_path Base directory path.
    /// \return Relative path string.
    template<typename Dummy = void>
    std::string makeRelative(const std::string& file_path, const std::string& base_path) {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "makeRelative is not supported on Emscripten");
        return file_path;
#else
        if (base_path.empty()) return file_path;

        fs::path file = fs::u8path(file_path);
        fs::path base = fs::u8path(base_path);

        std::error_code ec;
        fs::path rel = fs::relative(file, base, ec);
        return ec ? file_path : rel.u8string();
#endif
    }

    /// \brief Create directories recursively if missing.
    /// \tparam Dummy Dummy template parameter for SFINAE.
    /// \param path Directory path.
    /// \throws std::runtime_error when creation fails.
    template<typename Dummy = void>
    void createDirectories(const std::string& path) {
#ifdef __EMSCRIPTEN__
        static_assert(detail::dependent_false_v<Dummy>, "createDirectories is not supported on Emscripten");
        (void)path;
#else
        fs::path dir = fs::u8path(path);
        if (!fs::exists(dir)) {
            std::error_code ec;
            if (!fs::create_directories(dir, ec)) {
                throw std::runtime_error("Failed to create directories: " + dir.u8string());
            }
        }
#endif
    }

} // namespace ImGuiX::utils

#endif // _IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED
