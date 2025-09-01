#pragma once
#ifndef _IMGUIX_UTILS_BASE64_HPP_INCLUDED
#define _IMGUIX_UTILS_BASE64_HPP_INCLUDED

/// \file base64.hpp
/// \brief Base64 encoding and decoding helpers.

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ImGuiX::Utils {

    /// \brief Encode binary data to Base64 string.
    /// \param input Data to encode.
    /// \return Base64 representation.
    inline std::string encodeBase64(std::string_view input) {
        static const char kBase64Alphabet[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string output;
        output.reserve((input.size() + 2) / 3 * 4);

        for (size_t i = 0; i < input.size(); i += 3) {
            uint32_t buffer = 0;
            size_t remaining = std::min<size_t>(3, input.size() - i);

            for (size_t j = 0; j < remaining; ++j) {
                buffer |= static_cast<uint32_t>(
                    static_cast<unsigned char>(input[i + j])) << (16 - j * 8);
            }

            for (size_t j = 0; j < 4; ++j) {
                if (j <= remaining) {
                    output += kBase64Alphabet[(buffer >> (18 - j * 6)) & 0x3F];
                } else {
                    output += '=';
                }
            }
        }

        return output;
    }

    /// \brief Decode Base64 string back to binary data.
    /// \param input Base64 encoded string.
    /// \return Decoded bytes as string.
    /// \throws std::invalid_argument If input is not valid Base64.
    inline std::string decodeBase64(std::string_view input) {
        static const int kDecodingTable[128] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
        };

        std::string output;
        uint32_t buffer = 0;
        size_t bits_collected = 0;

        for (unsigned char c : input) {
            if (c == '=') {
                break;
            }
            if (c > 127 || kDecodingTable[c] == -1) {
                throw std::invalid_argument("Invalid Base64 input.");
            }

            buffer = (buffer << 6) | static_cast<uint32_t>(kDecodingTable[c]);
            bits_collected += 6;

            if (bits_collected >= 8) {
                bits_collected -= 8;
                output.push_back(static_cast<char>((buffer >> bits_collected) & 0xFF));
            }
        }

        return output;
    }

} // namespace ImGuiX::Utils

#endif // _IMGUIX_UTILS_BASE64_HPP_INCLUDED
