#include <imgui.h>
#include <imguix/core.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <imguix/themes/JsonBackedTheme.hpp>

#include <cstdint>
#include <cstdio>
#include <string>

#if defined(IMGUIX_USE_SFML_BACKEND)

namespace {

constexpr const char* kThemeId = "theme-custom-values-demo";
constexpr const char* kThemePath = "data/themes/theme-custom-values-demo.json";
constexpr const char* kCustomJsonSnippet =
R"json("custom": {
    "colors": {
        "by_name": {
            "SidebarAccountOnline": [0.35, 0.74, 0.39, 1.0]
        },
        "by_id": {
            "1001": [0.95, 0.71, 0.22, 1.0]
        }
    },
    "values": {
        "by_name": {
            "BrokerBadgeRounding": 6.0,
            "UseCompactHeader": true,
            "WidgetPadding": [10.0, 6.0],
            "WidgetCornerVector": [2.0, 2.0, 6.0, 6.0]
        },
        "by_id": {
            "2001": 12.0,
            "2002": false,
            "2003": [16.0, 10.0]
        }
    }
})json";

const char* sourceLabel(ImGuiX::Themes::ThemeValueSource source) {
    switch (source) {
        case ImGuiX::Themes::ThemeValueSource::NameKey:
            return "name key";
        case ImGuiX::Themes::ThemeValueSource::NumericId:
            return "numeric id";
        case ImGuiX::Themes::ThemeValueSource::Missing:
        default:
            return "local fallback";
    }
}

std::string formatVec2(const ImVec2& value) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "[%.2f, %.2f]", value.x, value.y);
    return std::string(buffer);
}

std::string formatVec4(const ImVec4& value) {
    char buffer[96];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "[%.2f, %.2f, %.2f, %.2f]",
        value.x,
        value.y,
        value.z,
        value.w);
    return std::string(buffer);
}

class ThemeCustomValuesController final : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {}

    void drawUi() override {
        ImGui::Begin("Theme Custom Values Demo");

        drawJsonSetup();
        ImGui::SeparatorText("Custom Colors");
        drawCustomColors();
        ImGui::SeparatorText("Custom Values");
        drawCustomValues();

        ImGui::End();
    }

    void drawJsonSetup() {
        ImGui::SeparatorText("JSON Setup");
        ImGui::Text("Theme file: %s", kThemePath);
        ImGui::TextUnformatted("The file is auto-generated on first run if it does not exist yet.");
        ImGui::TextUnformatted("Edit the JSON file, restart the demo, and the custom values will be picked up.");
        ImGui::TextUnformatted("Add or merge the following section into the generated file:");

        ImGui::BeginChild("custom-json-snippet", ImVec2(0.0f, 260.0f), true);
        ImGui::PushTextWrapPos();
        ImGui::TextUnformatted(kCustomJsonSnippet);
        ImGui::PopTextWrapPos();
        ImGui::EndChild();
    }

    void drawColorRow(const char* label, const ImGuiX::Themes::ResolvedThemeValue<ImVec4>& resolved) {
        ImGui::PushID(label);
        ImGui::ColorButton("##preview", resolved.value, ImGuiColorEditFlags_NoTooltip, ImVec2(42.0f, 18.0f));
        ImGui::SameLine();
        ImGui::Text(
            "%s | %s | %s",
            label,
            sourceLabel(resolved.source),
            formatVec4(resolved.value).c_str());
        ImGui::PopID();
    }

    void drawCustomColors() {
        const ImVec4 fallback_color(0.40f, 0.40f, 0.40f, 1.0f);

        const auto by_name = ImGuiX::Themes::resolveCurrentThemeCustomColor(
            themeManager(),
            "SidebarAccountOnline",
            fallback_color);
        const auto by_id = ImGuiX::Themes::resolveCurrentThemeCustomColor(
            themeManager(),
            1001U,
            fallback_color);
        const auto combined = ImGuiX::Themes::resolveCurrentThemeCustomColor(
            themeManager(),
            "SidebarAccountOnline",
            1001U,
            fallback_color);

        drawColorRow("By name: SidebarAccountOnline", by_name);
        drawColorRow("By id: 1001", by_id);
        drawColorRow("Combined: SidebarAccountOnline -> 1001", combined);

        ImGui::TextUnformatted(
            "If both SidebarAccountOnline and 1001 exist, the combined lookup uses SidebarAccountOnline.");
    }

    void drawCustomValues() {
        const float fallback_rounding = 4.0f;
        const bool fallback_compact_header = false;
        const ImVec2 fallback_padding(8.0f, 4.0f);
        const ImVec4 fallback_corner_vector(0.0f, 0.0f, 0.0f, 0.0f);

        const auto rounding_by_name = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "BrokerBadgeRounding",
            fallback_rounding);
        const auto rounding_by_id = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            2001U,
            fallback_rounding);
        const auto rounding_combined = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "BrokerBadgeRounding",
            2001U,
            fallback_rounding);

        const auto compact_header_by_name = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "UseCompactHeader",
            fallback_compact_header);
        const auto compact_header_by_id = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            2002U,
            fallback_compact_header);
        const auto compact_header_combined = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "UseCompactHeader",
            2002U,
            fallback_compact_header);

        const auto padding_by_name = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "WidgetPadding",
            fallback_padding);
        const auto padding_by_id = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            2003U,
            fallback_padding);
        const auto padding_combined = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "WidgetPadding",
            2003U,
            fallback_padding);

        const auto corner_vector_by_name = ImGuiX::Themes::resolveCurrentThemeCustomValue(
            themeManager(),
            "WidgetCornerVector",
            fallback_corner_vector);

        ImGui::Text(
            "By name: BrokerBadgeRounding -> %s | %.2f",
            sourceLabel(rounding_by_name.source),
            rounding_by_name.value);
        ImGui::Text(
            "By id: 2001 -> %s | %.2f",
            sourceLabel(rounding_by_id.source),
            rounding_by_id.value);
        ImGui::Text(
            "By name: UseCompactHeader -> %s | %s",
            sourceLabel(compact_header_by_name.source),
            compact_header_by_name.value ? "true" : "false");
        ImGui::Text(
            "By id: 2002 -> %s | %s",
            sourceLabel(compact_header_by_id.source),
            compact_header_by_id.value ? "true" : "false");
        ImGui::Text(
            "By name: WidgetPadding -> %s | %s",
            sourceLabel(padding_by_name.source),
            formatVec2(padding_by_name.value).c_str());
        ImGui::Text(
            "By id: 2003 -> %s | %s",
            sourceLabel(padding_by_id.source),
            formatVec2(padding_by_id.value).c_str());
        ImGui::Text(
            "By name: WidgetCornerVector -> %s | %s",
            sourceLabel(corner_vector_by_name.source),
            formatVec4(corner_vector_by_name.value).c_str());

        ImGui::Spacing();
        ImGui::TextUnformatted(
            "If both BrokerBadgeRounding and 2001 exist, the combined lookup uses BrokerBadgeRounding.");

        ImGui::BeginChild("custom-value-sample", ImVec2(0.0f, 220.0f), true);

        if (compact_header_combined.value) {
            ImGui::TextUnformatted("Compact Header | Demo Account | Live Theme Preview");
        } else {
            ImGui::TextUnformatted("Expanded Header");
            ImGui::TextUnformatted("Demo Account");
            ImGui::TextUnformatted("Live Theme Preview");
        }
        ImGui::Text("Header source: %s", sourceLabel(compact_header_combined.source));

        const ImVec4 accent_color = ImGuiX::Themes::resolveCurrentThemeCustomColor(
            themeManager(),
            "SidebarAccountOnline",
            1001U,
            ImVec4(0.40f, 0.40f, 0.40f, 1.0f)).value;
        ImGui::PushStyleColor(ImGuiCol_Button, accent_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accent_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, accent_color);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding_combined.value);
        ImGui::Button("Rounded Theme Badge");
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        ImGui::Text(
            "Rounding source: %s | %.2f",
            sourceLabel(rounding_combined.source),
            rounding_combined.value);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding_combined.value);
        ImGui::Button("Primary Action");
        ImGui::SameLine();
        ImGui::Button("Secondary");
        ImGui::SameLine();
        ImGui::Button("Inspect");
        ImGui::PopStyleVar();
        ImGui::Text(
            "Padding source: %s | %s",
            sourceLabel(padding_combined.source),
            formatVec2(padding_combined.value).c_str());

        ImGui::Text("Corner vector source: %s", sourceLabel(corner_vector_by_name.source));
        ImGui::Text("Corner vector value: %s", formatVec4(corner_vector_by_name.value).c_str());

        ImGui::EndChild();
    }

private:
};

class ThemeCustomValuesWindow final : public ImGuiX::WindowInstance {
public:
    ThemeCustomValuesWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<ThemeCustomValuesController>();
        create(980, 760);
        setWindowIcon("data/resources/icons/icon.png");

        auto& tm = themeManager();
        ImGuiX::Themes::registerJsonBackedTheme<ImGuiX::Themes::CorporateGreyTheme>(
            tm,
            kThemeId,
            kThemePath);
        setTheme(kThemeId);
        updateCurrentTheme();
    }
};

} // namespace

int main() {
    ImGuiX::Application app;
    app.createWindow<ThemeCustomValuesWindow>("Theme Custom Values Demo");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "theme_custom_values_demo.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
