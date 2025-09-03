#include <iostream>
#include <stdexcept>
#include <string>

#include <imguix/utils/base64.hpp>

int main() {
    const std::string original = "hello world";
    std::string encoded = ImGuiX::Utils::encodeBase64(original);
    std::string decoded = ImGuiX::Utils::decodeBase64(encoded);
    if (decoded != original) {
        std::cerr << "decode mismatch\n";
        return 1;
    }
    if (encoded != "aGVsbG8gd29ybGQ=") {
        std::cerr << "encode mismatch\n";
        return 1;
    }
    auto expect_throw = [](std::string_view input) {
        try {
            (void)ImGuiX::Utils::decodeBase64(input);
        } catch (const std::invalid_argument&) {
            return true;
        }
        return false;
    };

    if (!expect_throw("abc")) {
        std::cerr << "abc not rejected\n";
        return 1;
    }
    if (!expect_throw("====a")) {
        std::cerr << "====a not rejected\n";
        return 1;
    }
    return 0;
}
