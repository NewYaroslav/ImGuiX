#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <imgui.h>

#define private public
#include <imguix/core/fonts/FontManager.hpp>
#undef private

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

bool containsCodepoint(const std::vector<ImWchar>& ranges, ImWchar cp) {
    for (size_t i = 0; i + 1 < ranges.size(); i += 2) {
        if (ranges[i] == 0) {
            break;
        }
        if (cp >= ranges[i] && cp <= ranges[i + 1]) {
            return true;
        }
    }
    return false;
}

std::vector<ImWchar> buildWithPresetParser(const std::string& spec) {
    ImFontGlyphRangesBuilder builder;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiX::Fonts::FontManager::addNamedRanges(builder, io, spec);

    ImVector<ImWchar> built;
    builder.BuildRanges(&built);
    return std::vector<ImWchar>(built.begin(), built.end());
}

std::vector<ImWchar> buildWithPackPreset(const std::string& spec) {
    ImGuiX::Fonts::LocalePack pack{};
    pack.ranges_preset = spec;

    std::vector<ImWchar> out;
    ImGuiX::Fonts::FontManager::buildRangesFromPack(out, &pack, "default");
    return out;
}

std::vector<ImWchar> buildWithPackRoles(
    const std::string& spec,
    const std::unordered_map<ImGuiX::Fonts::FontRole, std::vector<ImGuiX::Fonts::FontFile>>& roles) {
    ImGuiX::Fonts::LocalePack pack{};
    pack.ranges_preset = spec;
    pack.roles = roles;

    std::vector<ImWchar> out;
    ImGuiX::Fonts::FontManager::buildRangesFromPack(out, &pack, "default");
    return out;
}

} // namespace

int main() {
    ImGuiContext* ctx = ImGui::CreateContext();
    require(ctx != nullptr, "failed to create ImGui context");

    {
        const auto ranges = buildWithPresetParser("Default+Cyrillic");
        require(containsCodepoint(ranges, static_cast<ImWchar>('A')), "named preset lost default latin glyphs");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x0416)), "named preset lost cyrillic glyphs");
    }

    {
        const auto ranges = buildWithPresetParser("Default+LatinExtA+0x2010-0x205E+0xE000-0xF8FF");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x017D)), "mixed hex preset missed LatinExtA glyph");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x203B)), "mixed hex preset missed numeric punctuation range");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0xE123)), "mixed hex preset missed PUA range");
    }

    {
        const auto ranges = buildWithPresetParser("Default+1024-1327+57344-63743");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x041F)), "mixed decimal preset missed decimal cyrillic range");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0xE321)), "mixed decimal preset missed decimal PUA range");
    }

    {
        const auto ranges = buildWithPresetParser("Default+8230+0x2022");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x2026)), "single decimal codepoint token was not parsed");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x2022)), "single hex codepoint token was not parsed");
    }

    {
        const auto ranges = buildWithPresetParser("Default+0xXYZ+123-abc+0x2026+57344-63743");
        require(containsCodepoint(ranges, static_cast<ImWchar>('A')), "invalid numeric token broke valid named token");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x2026)), "valid numeric codepoint after invalid token was dropped");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0xE222)), "valid numeric range after invalid token was dropped");
    }

    {
        const auto ranges = buildWithPackPreset("Default+LatinExtA+0x203B+0xE000-0xF8FF");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x017D)), "pack preset string missed named token");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x203B)), "pack preset string missed single numeric token");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0xE100)), "pack preset string missed numeric range token");
    }

    {
        ImGuiX::Fonts::FontFile body_font{};
        body_font.extra_glyphs = u8"\u2713";

        ImGuiX::Fonts::FontFile merged_symbols{};
        merged_symbols.merge = true;
        merged_symbols.extra_glyphs = u8"\u2103";

        std::unordered_map<ImGuiX::Fonts::FontRole, std::vector<ImGuiX::Fonts::FontFile>> roles;
        roles[ImGuiX::Fonts::FontRole::Body].push_back(body_font);
        roles[ImGuiX::Fonts::FontRole::Emoji].push_back(merged_symbols);

        const auto ranges = buildWithPackRoles("Default+LatinExtA", roles);
        require(containsCodepoint(ranges, static_cast<ImWchar>('A')), "extra_glyphs changed valid preset coverage");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x017D)), "extra_glyphs changed named preset coverage");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x2713)), "body extra_glyphs were not added to final ranges");
        require(containsCodepoint(ranges, static_cast<ImWchar>(0x2103)), "merged role extra_glyphs were not unioned into final ranges");
    }

    ImGui::DestroyContext(ctx);
    return 0;
}
