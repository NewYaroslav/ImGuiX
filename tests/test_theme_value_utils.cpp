#include <imguix/core.hpp>

#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace {

bool nearlyEqual(float lhs, float rhs) {
    return std::fabs(lhs - rhs) < 0.0001f;
}

bool equalVec2(const ImVec2& lhs, const ImVec2& rhs) {
    return nearlyEqual(lhs.x, rhs.x) && nearlyEqual(lhs.y, rhs.y);
}

bool equalVec4(const ImVec4& lhs, const ImVec4& rhs) {
    return nearlyEqual(lhs.x, rhs.x) &&
           nearlyEqual(lhs.y, rhs.y) &&
           nearlyEqual(lhs.z, rhs.z) &&
           nearlyEqual(lhs.w, rhs.w);
}

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

class FakeTheme final : public ImGuiX::Themes::Theme {
public:
    void apply(ImGuiStyle& style) const override {
        (void)style;
    }

#ifdef IMGUIX_ENABLE_IMPLOT
    void apply(ImPlotStyle& style) const override {
        (void)style;
    }
#endif

#ifdef IMGUIX_ENABLE_IMPLOT3D
    void apply(ImPlot3DStyle& style) const override {
        (void)style;
    }
#endif

    bool tryGetCustomColor(std::string_view key, ImVec4& out) const override {
        const auto it = m_colors_by_name.find(std::string(key));
        if (it == m_colors_by_name.end()) {
            return false;
        }
        out = it->second;
        return true;
    }

    bool tryGetCustomColor(std::uint32_t key, ImVec4& out) const override {
        const auto it = m_colors_by_id.find(key);
        if (it == m_colors_by_id.end()) {
            return false;
        }
        out = it->second;
        return true;
    }

    bool tryGetCustomValue(std::string_view key, ImGuiX::Themes::ThemeCustomValue& out) const override {
        const auto it = m_values_by_name.find(std::string(key));
        if (it == m_values_by_name.end()) {
            return false;
        }
        out = it->second;
        return true;
    }

    bool tryGetCustomValue(std::uint32_t key, ImGuiX::Themes::ThemeCustomValue& out) const override {
        const auto it = m_values_by_id.find(key);
        if (it == m_values_by_id.end()) {
            return false;
        }
        out = it->second;
        return true;
    }

    std::unordered_map<std::string, ImVec4> m_colors_by_name;
    std::unordered_map<std::uint32_t, ImVec4> m_colors_by_id;
    std::unordered_map<std::string, ImGuiX::Themes::ThemeCustomValue> m_values_by_name;
    std::unordered_map<std::uint32_t, ImGuiX::Themes::ThemeCustomValue> m_values_by_id;
};

FakeTheme& registerFakeTheme(ImGuiX::Themes::ThemeManager& theme_manager) {
    auto fake_theme = std::make_unique<FakeTheme>();
    FakeTheme& fake_theme_ref = *fake_theme;
    theme_manager.registerTheme("fake-theme-values", std::move(fake_theme));
    theme_manager.setTheme("fake-theme-values");
    return fake_theme_ref;
}

} // namespace

int main() {
    using ImGuiX::Themes::ResolvedThemeValue;
    using ImGuiX::Themes::ThemeCustomValue;
    using ImGuiX::Themes::ThemeManager;
    using ImGuiX::Themes::ThemeValueSource;
    using ImGuiX::Themes::resolveCurrentThemeCustomColor;
    using ImGuiX::Themes::resolveCurrentThemeCustomValue;
    using ImGuiX::Themes::tryExtractThemeValue;

    {
        ThemeCustomValue value = 4.5f;
        float resolved = 0.0f;
        require(tryExtractThemeValue(value, resolved), "float extraction from float failed");
        require(nearlyEqual(resolved, 4.5f), "float extraction from float mismatch");
    }

    {
        ThemeCustomValue value = 7;
        float resolved = 0.0f;
        require(tryExtractThemeValue(value, resolved), "float extraction from int failed");
        require(nearlyEqual(resolved, 7.0f), "float extraction from int mismatch");
    }

    {
        ThemeCustomValue value = 9.0f;
        int resolved = 0;
        require(!tryExtractThemeValue(value, resolved), "int extraction accepted float");
    }

    {
        ThemeCustomValue value = true;
        bool resolved = false;
        require(tryExtractThemeValue(value, resolved) && resolved, "bool extraction failed");
    }

    {
        ThemeCustomValue value = ImVec2(1.0f, 2.0f);
        ImVec2 resolved(0.0f, 0.0f);
        require(tryExtractThemeValue(value, resolved), "ImVec2 extraction failed");
        require(equalVec2(resolved, ImVec2(1.0f, 2.0f)), "ImVec2 extraction mismatch");
    }

    {
        ThemeCustomValue value = ImVec4(1.0f, 2.0f, 3.0f, 4.0f);
        ImVec4 resolved(0.0f, 0.0f, 0.0f, 0.0f);
        require(tryExtractThemeValue(value, resolved), "ImVec4 extraction failed");
        require(equalVec4(resolved, ImVec4(1.0f, 2.0f, 3.0f, 4.0f)), "ImVec4 extraction mismatch");
    }

    ThemeManager theme_manager;
    FakeTheme& fake_theme = registerFakeTheme(theme_manager);

    fake_theme.m_colors_by_name["name-color"] = ImVec4(0.10f, 0.20f, 0.30f, 1.0f);
    fake_theme.m_colors_by_id[1001U] = ImVec4(0.90f, 0.80f, 0.70f, 1.0f);
    fake_theme.m_colors_by_name["shared-color"] = ImVec4(0.11f, 0.22f, 0.33f, 1.0f);
    fake_theme.m_colors_by_id[1002U] = ImVec4(0.91f, 0.82f, 0.73f, 1.0f);

    fake_theme.m_values_by_name["float-name"] = 5.5f;
    fake_theme.m_values_by_id[2001U] = 6;
    fake_theme.m_values_by_name["int-float-mismatch"] = 7.25f;
    fake_theme.m_values_by_name["bool-name"] = true;
    fake_theme.m_values_by_name["padding-name"] = ImVec2(3.0f, 4.0f);
    fake_theme.m_values_by_name["vector-name"] = ImVec4(5.0f, 6.0f, 7.0f, 8.0f);
    fake_theme.m_values_by_name["shared-rounding"] = true;
    fake_theme.m_values_by_id[2002U] = 14;
    fake_theme.m_values_by_name["color-like-value"] = ImVec4(0.2f, 0.3f, 0.4f, 1.0f);
    fake_theme.m_colors_by_name["value-like-color"] = ImVec4(0.7f, 0.6f, 0.5f, 1.0f);

    const ImVec4 fallback_color(0.4f, 0.4f, 0.4f, 1.0f);

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, "name-color", fallback_color);
        require(resolved.source == ThemeValueSource::NameKey, "color by name source mismatch");
        require(equalVec4(resolved.value, ImVec4(0.10f, 0.20f, 0.30f, 1.0f)), "color by name value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, 1001U, fallback_color);
        require(resolved.source == ThemeValueSource::NumericId, "color by id source mismatch");
        require(equalVec4(resolved.value, ImVec4(0.90f, 0.80f, 0.70f, 1.0f)), "color by id value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, "shared-color", 1002U, fallback_color);
        require(resolved.source == ThemeValueSource::NameKey, "combined color precedence source mismatch");
        require(equalVec4(resolved.value, ImVec4(0.11f, 0.22f, 0.33f, 1.0f)), "combined color precedence value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, "missing-color", 1001U, fallback_color);
        require(resolved.source == ThemeValueSource::NumericId, "combined color id fallback source mismatch");
        require(equalVec4(resolved.value, ImVec4(0.90f, 0.80f, 0.70f, 1.0f)), "combined color id fallback value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, "missing-color", 9999U, fallback_color);
        require(resolved.source == ThemeValueSource::Missing, "combined color fallback source mismatch");
        require(equalVec4(resolved.value, fallback_color), "combined color fallback value mismatch");
    }

    {
        const ResolvedThemeValue<float> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "float-name", 1.0f);
        require(resolved.source == ThemeValueSource::NameKey, "float value by name source mismatch");
        require(nearlyEqual(resolved.value, 5.5f), "float value by name mismatch");
    }

    {
        const ResolvedThemeValue<float> resolved =
            resolveCurrentThemeCustomValue(theme_manager, 2001U, 1.0f);
        require(resolved.source == ThemeValueSource::NumericId, "float value by id source mismatch");
        require(nearlyEqual(resolved.value, 6.0f), "float value by id mismatch");
    }

    {
        const ResolvedThemeValue<int> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "int-float-mismatch", 42);
        require(resolved.source == ThemeValueSource::Missing, "int mismatch source mismatch");
        require(resolved.value == 42, "int mismatch fallback mismatch");
    }

    {
        const ResolvedThemeValue<bool> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "bool-name", false);
        require(resolved.source == ThemeValueSource::NameKey, "bool value source mismatch");
        require(resolved.value, "bool value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec2> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "padding-name", ImVec2(0.0f, 0.0f));
        require(resolved.source == ThemeValueSource::NameKey, "ImVec2 source mismatch");
        require(equalVec2(resolved.value, ImVec2(3.0f, 4.0f)), "ImVec2 value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "vector-name", ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        require(resolved.source == ThemeValueSource::NameKey, "ImVec4 source mismatch");
        require(equalVec4(resolved.value, ImVec4(5.0f, 6.0f, 7.0f, 8.0f)), "ImVec4 value mismatch");
    }

    {
        const ResolvedThemeValue<int> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "shared-rounding", 2002U, 2);
        require(resolved.source == ThemeValueSource::NumericId, "combined value mismatch fallback source mismatch");
        require(resolved.value == 14, "combined value mismatch fallback value mismatch");
    }

    {
        const ResolvedThemeValue<ImVec4> resolved =
            resolveCurrentThemeCustomColor(theme_manager, "color-like-value", fallback_color);
        require(resolved.source == ThemeValueSource::Missing, "values leaked into color helper");
        require(equalVec4(resolved.value, fallback_color), "color helper fallback mismatch");
    }

    {
        const ResolvedThemeValue<float> resolved =
            resolveCurrentThemeCustomValue(theme_manager, "value-like-color", 3.0f);
        require(resolved.source == ThemeValueSource::Missing, "colors leaked into value helper");
        require(nearlyEqual(resolved.value, 3.0f), "value helper fallback mismatch");
    }

    return 0;
}
