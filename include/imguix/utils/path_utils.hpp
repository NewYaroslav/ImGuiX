#pragma once
#ifndef IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED
#define IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED

/// \file path_utils.hpp
/// \brief Path utilities for resolving executable-relative paths and file manipulations.

#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

#ifdef _WIN32
#   include <Windows.h>
#   include <codecvt>
#   include <locale>
#else
#   include <limits.h>
#   include <unistd.h>
#endif

namespace ImGuiX::Utils {
    namespace fs = std::filesystem;

    /// \brief Returns full path to the current executable.
    std::string getExecPath() {
#ifdef _WIN32
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
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count == -1)
            throw std::runtime_error("Failed to get executable path.");

        return std::string(result, count);
#endif
    }

    /// \brief Returns directory path of the executable.
    std::string getExecDir() {
        const std::string path = getExecPath();
        return fs::path(path).parent_path().string();
    }

    /// \brief Resolves a relative path to absolute, based on executable location.
    /// \param relative_path Relative path from executable directory.
    /// \return Absolute path string.
    std::string resolveExecPath(const std::string& relative_path) {
        return (fs::path(getExecDir()) / relative_path).string();
    }

    /// \brief Extracts filename from full path.
    std::string getFileName(const std::string& full_path) {
        return fs::path(full_path).filename().string();
    }

    /// \brief Computes relative path from base to target.
    std::string makeRelative(const std::string& file_path, const std::string& base_path) {
        if (base_path.empty()) return file_path;

        fs::path file = fs::u8path(file_path);
        fs::path base = fs::u8path(base_path);

        std::error_code ec;
        fs::path rel = fs::relative(file, base, ec);
        return ec ? file_path : rel.u8string();
    }

    /// \brief Recursively creates directory if it doesn't exist.
    void createDirectories(const std::string& path) {
        fs::path dir(path);
        if (!fs::exists(dir)) {
            std::error_code ec;
            if (!fs::create_directories(dir, ec)) {
                throw std::runtime_error("Failed to create directories: " + path);
            }
        }
    }

} // namespace ImGuiX::utils

#endif // IMGUIX_UTILS_PATH_UTILS_HPP_INCLUDED