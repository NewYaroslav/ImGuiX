#include <imguix/utils/base64.hpp>
#include <iostream>
#include <string>

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
    return 0;
}
