#pragma once
#ifndef _IMGUIX_THEMES_JSON_BACKED_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_JSON_BACKED_THEME_HPP_INCLUDED

/// \file JsonBackedTheme.hpp
/// \brief JSON-backed theme wrapper for file-based style overrides.

#include <imguix/core/themes/Theme.hpp>
#include <imguix/utils/strip_json_comments.hpp>

#include <nlohmann/json.hpp>

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <limits>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>

namespace ImGuiX::Themes {

    namespace detail {

        using Json = nlohmann::ordered_json;

        enum class ThemeFieldType {
            Float,
            Int,
            Bool,
            Vec2
        };

        struct ThemeFieldSpec {
            const char* name;
            ThemeFieldType type;
            std::size_t offset;
        };

#define IMGUIX_THEME_FLOAT_FIELD(Type, Field) {#Field, ThemeFieldType::Float, offsetof(Type, Field)}
#define IMGUIX_THEME_INT_FIELD(Type, Field) {#Field, ThemeFieldType::Int, offsetof(Type, Field)}
#define IMGUIX_THEME_BOOL_FIELD(Type, Field) {#Field, ThemeFieldType::Bool, offsetof(Type, Field)}
#define IMGUIX_THEME_VEC2_FIELD(Type, Field) {#Field, ThemeFieldType::Vec2, offsetof(Type, Field)}

        inline constexpr ThemeFieldSpec IMGUI_STYLE_FIELDS[] = {
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, FontSizeBase),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, FontScaleMain),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, FontScaleDpi),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, Alpha),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, DisabledAlpha),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, WindowPadding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, WindowRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, WindowBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, WindowBorderHoverPadding),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, WindowMinSize),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, WindowTitleAlign),
            IMGUIX_THEME_INT_FIELD(ImGuiStyle, WindowMenuButtonPosition),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ChildRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ChildBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, PopupRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, PopupBorderSize),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, FramePadding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, FrameRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, FrameBorderSize),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, ItemSpacing),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, ItemInnerSpacing),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, CellPadding),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, TouchExtraPadding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, IndentSpacing),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ColumnsMinSpacing),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ScrollbarSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ScrollbarRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, GrabMinSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, GrabRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, LogSliderDeadzone),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, ImageBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabRounding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabCloseButtonMinWidthSelected),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabCloseButtonMinWidthUnselected),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabBarBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TabBarOverlineSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TableAngledHeadersAngle),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, TableAngledHeadersTextAlign),
            IMGUIX_THEME_INT_FIELD(ImGuiStyle, TreeLinesFlags),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TreeLinesSize),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, TreeLinesRounding),
            IMGUIX_THEME_INT_FIELD(ImGuiStyle, ColorButtonPosition),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, ButtonTextAlign),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, SelectableTextAlign),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, SeparatorTextBorderSize),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, SeparatorTextAlign),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, SeparatorTextPadding),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, DisplayWindowPadding),
            IMGUIX_THEME_VEC2_FIELD(ImGuiStyle, DisplaySafeAreaPadding),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, MouseCursorScale),
            IMGUIX_THEME_BOOL_FIELD(ImGuiStyle, AntiAliasedLines),
            IMGUIX_THEME_BOOL_FIELD(ImGuiStyle, AntiAliasedLinesUseTex),
            IMGUIX_THEME_BOOL_FIELD(ImGuiStyle, AntiAliasedFill),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, CurveTessellationTol),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, CircleTessellationMaxError),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, HoverStationaryDelay),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, HoverDelayShort),
            IMGUIX_THEME_FLOAT_FIELD(ImGuiStyle, HoverDelayNormal),
            IMGUIX_THEME_INT_FIELD(ImGuiStyle, HoverFlagsForTooltipMouse),
            IMGUIX_THEME_INT_FIELD(ImGuiStyle, HoverFlagsForTooltipNav),
        };

#ifdef IMGUIX_ENABLE_IMPLOT
        inline constexpr ThemeFieldSpec IMPLOT_STYLE_FIELDS[] = {
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, LineWeight),
            IMGUIX_THEME_INT_FIELD(ImPlotStyle, Marker),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, MarkerSize),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, MarkerWeight),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, FillAlpha),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, ErrorBarSize),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, ErrorBarWeight),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, DigitalBitHeight),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, DigitalBitGap),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, PlotBorderSize),
            IMGUIX_THEME_FLOAT_FIELD(ImPlotStyle, MinorAlpha),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MajorTickLen),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MinorTickLen),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MajorTickSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MinorTickSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MajorGridSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MinorGridSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, PlotPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, LabelPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, LegendPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, LegendInnerPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, LegendSpacing),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, MousePosPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, AnnotationPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, FitPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, PlotDefaultSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlotStyle, PlotMinSize),
            IMGUIX_THEME_INT_FIELD(ImPlotStyle, Colormap),
            IMGUIX_THEME_BOOL_FIELD(ImPlotStyle, UseLocalTime),
            IMGUIX_THEME_BOOL_FIELD(ImPlotStyle, UseISO8601),
            IMGUIX_THEME_BOOL_FIELD(ImPlotStyle, Use24HourClock),
        };
#endif

#ifdef IMGUIX_ENABLE_IMPLOT3D
        inline constexpr ThemeFieldSpec IMPLOT3D_STYLE_FIELDS[] = {
            IMGUIX_THEME_FLOAT_FIELD(ImPlot3DStyle, LineWeight),
            IMGUIX_THEME_INT_FIELD(ImPlot3DStyle, Marker),
            IMGUIX_THEME_FLOAT_FIELD(ImPlot3DStyle, MarkerSize),
            IMGUIX_THEME_FLOAT_FIELD(ImPlot3DStyle, MarkerWeight),
            IMGUIX_THEME_FLOAT_FIELD(ImPlot3DStyle, FillAlpha),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, PlotDefaultSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, PlotMinSize),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, PlotPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, LabelPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, LegendPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, LegendInnerPadding),
            IMGUIX_THEME_VEC2_FIELD(ImPlot3DStyle, LegendSpacing),
            IMGUIX_THEME_INT_FIELD(ImPlot3DStyle, Colormap),
        };

        inline constexpr const char* IMPLOT3D_COLOR_NAMES[] = {
            "Line",
            "Fill",
            "MarkerOutline",
            "MarkerFill",
            "TitleText",
            "InlayText",
            "FrameBg",
            "PlotBg",
            "PlotBorder",
            "LegendBg",
            "LegendBorder",
            "LegendText",
            "AxisText",
            "AxisGrid",
            "AxisTick",
        };
#endif

#undef IMGUIX_THEME_FLOAT_FIELD
#undef IMGUIX_THEME_INT_FIELD
#undef IMGUIX_THEME_BOOL_FIELD
#undef IMGUIX_THEME_VEC2_FIELD

        template<typename TStyle>
        inline void* field_ptr(TStyle& style, std::size_t offset) {
            return reinterpret_cast<void*>(reinterpret_cast<char*>(&style) + offset);
        }

        template<typename TStyle>
        inline const void* field_ptr(const TStyle& style, std::size_t offset) {
            return reinterpret_cast<const void*>(reinterpret_cast<const char*>(&style) + offset);
        }

        inline std::string make_prefixed_color_name(const char* prefix, const char* name) {
            return std::string(prefix) + name;
        }

        struct ScopedImGuiStyleOverride {
            explicit ScopedImGuiStyleOverride(const ImGuiStyle& replacement_style)
                : m_original_style(ImGui::GetStyle()) {
                ImGui::GetStyle() = replacement_style;
            }

            ~ScopedImGuiStyleOverride() {
                ImGui::GetStyle() = m_original_style;
            }

            ScopedImGuiStyleOverride(const ScopedImGuiStyleOverride&) = delete;
            ScopedImGuiStyleOverride& operator=(const ScopedImGuiStyleOverride&) = delete;

        private:
            ImGuiStyle m_original_style;
        };

        inline bool json_to_float(const Json& value, float& out_value) {
            if (!value.is_number()) {
                return false;
            }
            out_value = value.get<float>();
            return true;
        }

        inline bool json_to_int(const Json& value, int& out_value) {
            if (value.is_number_integer() || value.is_number_unsigned()) {
                out_value = value.get<int>();
                return true;
            }
            if (!value.is_number_float()) {
                return false;
            }
            const double numeric_value = value.get<double>();
            const double integral_part = std::trunc(numeric_value);
            if (numeric_value != integral_part) {
                return false;
            }
            out_value = static_cast<int>(integral_part);
            return true;
        }

        inline bool json_to_bool(const Json& value, bool& out_value) {
            if (!value.is_boolean()) {
                return false;
            }
            out_value = value.get<bool>();
            return true;
        }

        inline bool json_to_vec2(const Json& value, ImVec2& out_value) {
            if (!value.is_array() || value.size() != 2U || !value[0].is_number() || !value[1].is_number()) {
                return false;
            }
            out_value = ImVec2(value[0].get<float>(), value[1].get<float>());
            return true;
        }

        inline bool json_to_vec4(const Json& value, ImVec4& out_value) {
            if (!value.is_array() ||
                value.size() != 4U ||
                !value[0].is_number() ||
                !value[1].is_number() ||
                !value[2].is_number() ||
                !value[3].is_number()) {
                return false;
            }
            out_value = ImVec4(
                value[0].get<float>(),
                value[1].get<float>(),
                value[2].get<float>(),
                value[3].get<float>());
            return true;
        }

        inline Json vec2_to_json(const ImVec2& value) {
            return Json::array({value.x, value.y});
        }

        inline Json vec4_to_json(const ImVec4& value) {
            return Json::array({value.x, value.y, value.z, value.w});
        }

        inline Json serialize_empty_custom_group() {
            Json custom_group = Json::object();
            custom_group["colors"] = Json::object({
                {"by_name", Json::object()},
                {"by_id", Json::object()},
            });
            custom_group["values"] = Json::object({
                {"by_name", Json::object()},
                {"by_id", Json::object()},
            });
            return custom_group;
        }

        inline bool parse_uint32_key(const std::string& key, std::uint32_t& out_value) {
            if (key.empty()) {
                return false;
            }
            std::size_t parsed_chars = 0;
            try {
                const unsigned long raw_value = std::stoul(key, &parsed_chars, 10);
                if (parsed_chars != key.size() ||
                    raw_value > static_cast<unsigned long>(std::numeric_limits<std::uint32_t>::max())) {
                    return false;
                }
                out_value = static_cast<std::uint32_t>(raw_value);
                return true;
            } catch (...) {
                return false;
            }
        }

        inline bool json_to_custom_value(const Json& value, ThemeCustomValue& out_value) {
            if (value.is_boolean()) {
                out_value = value.get<bool>();
                return true;
            }
            int int_value = 0;
            if (json_to_int(value, int_value)) {
                out_value = int_value;
                return true;
            }
            if (value.is_number_float()) {
                out_value = value.get<float>();
                return true;
            }

            ImVec2 vec2_value;
            if (json_to_vec2(value, vec2_value)) {
                out_value = vec2_value;
                return true;
            }

            ImVec4 vec4_value;
            if (json_to_vec4(value, vec4_value)) {
                out_value = vec4_value;
                return true;
            }

            return false;
        }

        inline void parse_custom_color_name_map(
                const Json& object_json,
                std::unordered_map<std::string, ImVec4>& out_values) {
            if (!object_json.is_object()) {
                return;
            }
            for (auto it = object_json.begin(); it != object_json.end(); ++it) {
                ImVec4 value;
                if (json_to_vec4(it.value(), value)) {
                    out_values[it.key()] = value;
                }
            }
        }

        inline void parse_custom_color_id_map(
                const Json& object_json,
                std::unordered_map<std::uint32_t, ImVec4>& out_values) {
            if (!object_json.is_object()) {
                return;
            }
            for (auto it = object_json.begin(); it != object_json.end(); ++it) {
                std::uint32_t id = 0;
                ImVec4 value;
                if (parse_uint32_key(it.key(), id) && json_to_vec4(it.value(), value)) {
                    out_values[id] = value;
                }
            }
        }

        inline void parse_custom_value_name_map(
                const Json& object_json,
                std::unordered_map<std::string, ThemeCustomValue>& out_values) {
            if (!object_json.is_object()) {
                return;
            }
            for (auto it = object_json.begin(); it != object_json.end(); ++it) {
                ThemeCustomValue value;
                if (json_to_custom_value(it.value(), value)) {
                    out_values[it.key()] = std::move(value);
                }
            }
        }

        inline void parse_custom_value_id_map(
                const Json& object_json,
                std::unordered_map<std::uint32_t, ThemeCustomValue>& out_values) {
            if (!object_json.is_object()) {
                return;
            }
            for (auto it = object_json.begin(); it != object_json.end(); ++it) {
                std::uint32_t id = 0;
                ThemeCustomValue value;
                if (parse_uint32_key(it.key(), id) && json_to_custom_value(it.value(), value)) {
                    out_values[id] = std::move(value);
                }
            }
        }

        inline void parse_custom_theme_data(
                const Json& root_json,
                std::unordered_map<std::string, ImVec4>& custom_colors_by_name,
                std::unordered_map<std::uint32_t, ImVec4>& custom_colors_by_id,
                std::unordered_map<std::string, ThemeCustomValue>& custom_values_by_name,
                std::unordered_map<std::uint32_t, ThemeCustomValue>& custom_values_by_id) {
            custom_colors_by_name.clear();
            custom_colors_by_id.clear();
            custom_values_by_name.clear();
            custom_values_by_id.clear();

            const auto custom_it = root_json.find("custom");
            if (custom_it == root_json.end() || !custom_it->is_object()) {
                return;
            }

            const auto colors_it = custom_it->find("colors");
            if (colors_it != custom_it->end() && colors_it->is_object()) {
                const auto by_name_it = colors_it->find("by_name");
                if (by_name_it != colors_it->end()) {
                    parse_custom_color_name_map(*by_name_it, custom_colors_by_name);
                }
                const auto by_id_it = colors_it->find("by_id");
                if (by_id_it != colors_it->end()) {
                    parse_custom_color_id_map(*by_id_it, custom_colors_by_id);
                }
            }

            const auto values_it = custom_it->find("values");
            if (values_it != custom_it->end() && values_it->is_object()) {
                const auto by_name_it = values_it->find("by_name");
                if (by_name_it != values_it->end()) {
                    parse_custom_value_name_map(*by_name_it, custom_values_by_name);
                }
                const auto by_id_it = values_it->find("by_id");
                if (by_id_it != values_it->end()) {
                    parse_custom_value_id_map(*by_id_it, custom_values_by_id);
                }
            }
        }

        inline std::string format_json_scalar(const Json& value);

        inline std::string format_float_value(double value) {
            const double rounded_value = std::round(value * 10000.0) / 10000.0;
            std::ostringstream output;
            output << std::fixed << std::setprecision(4) << rounded_value;
            std::string result = output.str();

            const std::size_t decimal_pos = result.find('.');
            if (decimal_pos == std::string::npos) {
                return result;
            }

            while (result.size() > decimal_pos + 2U && result.back() == '0') {
                result.pop_back();
            }
            if (result.back() == '.') {
                result.push_back('0');
            }
            return result;
        }

        inline bool is_scalar_json(const Json& value) {
            return value.is_primitive();
        }

        inline bool should_format_array_inline(const Json& value) {
            if (!value.is_array() || (value.size() != 2U && value.size() != 4U)) {
                return false;
            }
            for (const auto& item : value) {
                if (!is_scalar_json(item)) {
                    return false;
                }
            }
            return true;
        }

        inline void append_indent(std::string& output, int indent) {
            output.append(static_cast<std::size_t>(indent), ' ');
        }

        inline void append_formatted_json(std::string& output, const Json& value, int indent) {
            if (value.is_object()) {
                if (value.empty()) {
                    output += "{}";
                    return;
                }

                output += "{\n";
                bool is_first = true;
                for (auto it = value.begin(); it != value.end(); ++it) {
                    if (!is_first) {
                        output += ",\n";
                    }
                    is_first = false;
                    append_indent(output, indent + 4);
                    output += Json(it.key()).dump();
                    output += ": ";
                    append_formatted_json(output, it.value(), indent + 4);
                }
                output += '\n';
                append_indent(output, indent);
                output += '}';
                return;
            }

            if (value.is_array()) {
                if (value.empty()) {
                    output += "[]";
                    return;
                }

                if (should_format_array_inline(value)) {
                    output += "[";
                    for (std::size_t index = 0; index < value.size(); ++index) {
                        if (index != 0U) {
                            output += ", ";
                        }
                        output += format_json_scalar(value[index]);
                    }
                    output += "]";
                    return;
                }

                output += "[\n";
                for (std::size_t index = 0; index < value.size(); ++index) {
                    if (index != 0U) {
                        output += ",\n";
                    }
                    append_indent(output, indent + 4);
                    append_formatted_json(output, value[index], indent + 4);
                }
                output += '\n';
                append_indent(output, indent);
                output += ']';
                return;
            }

            output += format_json_scalar(value);
        }

        inline std::string format_json_scalar(const Json& value) {
            if (value.is_boolean()) {
                return value.get<bool>() ? "true" : "false";
            }
            if (value.is_number_integer()) {
                return std::to_string(value.get<long long>());
            }
            if (value.is_number_unsigned()) {
                return std::to_string(value.get<unsigned long long>());
            }
            if (value.is_number_float()) {
                return format_float_value(value.get<double>());
            }
            return value.dump();
        }

        inline std::string format_json_document(const Json& document) {
            std::string output;
            append_formatted_json(output, document, 0);
            output.push_back('\n');
            return output;
        }

        template<typename TStyle>
        inline Json serialize_field_value(const TStyle& style, const ThemeFieldSpec& spec) {
            const void* raw_field = field_ptr(style, spec.offset);
            switch (spec.type) {
                case ThemeFieldType::Float:
                    return Json(*reinterpret_cast<const float*>(raw_field));
                case ThemeFieldType::Int:
                    return Json(*reinterpret_cast<const int*>(raw_field));
                case ThemeFieldType::Bool:
                    return Json(*reinterpret_cast<const bool*>(raw_field));
                case ThemeFieldType::Vec2:
                    return vec2_to_json(*reinterpret_cast<const ImVec2*>(raw_field));
            }
            return Json();
        }

        template<typename TStyle>
        inline void apply_field_override(
                TStyle& style,
                const ThemeFieldSpec& spec,
                const Json& style_json) {
            if (!style_json.is_object()) {
                return;
            }
            const auto field_it = style_json.find(spec.name);
            if (field_it == style_json.end()) {
                return;
            }

            void* raw_field = field_ptr(style, spec.offset);
            switch (spec.type) {
                case ThemeFieldType::Float: {
                    float value = 0.0f;
                    if (json_to_float(*field_it, value)) {
                        *reinterpret_cast<float*>(raw_field) = value;
                    }
                    return;
                }
                case ThemeFieldType::Int: {
                    int value = 0;
                    if (json_to_int(*field_it, value)) {
                        *reinterpret_cast<int*>(raw_field) = value;
                    }
                    return;
                }
                case ThemeFieldType::Bool: {
                    bool value = false;
                    if (json_to_bool(*field_it, value)) {
                        *reinterpret_cast<bool*>(raw_field) = value;
                    }
                    return;
                }
                case ThemeFieldType::Vec2: {
                    ImVec2 value;
                    if (json_to_vec2(*field_it, value)) {
                        *reinterpret_cast<ImVec2*>(raw_field) = value;
                    }
                    return;
                }
            }
        }

        template<typename TStyle, std::size_t TSize>
        inline Json serialize_style_group(
                const TStyle& style,
                const ThemeFieldSpec (&specs)[TSize]) {
            Json style_json = Json::object();
            for (const ThemeFieldSpec& spec : specs) {
                style_json[spec.name] = serialize_field_value(style, spec);
            }
            return style_json;
        }

        template<typename TStyle, std::size_t TSize>
        inline void apply_style_group_overrides(
                TStyle& style,
                const Json& root_json,
                const char* root_key,
                const ThemeFieldSpec (&specs)[TSize]) {
            const auto root_it = root_json.find(root_key);
            if (root_it == root_json.end() || !root_it->is_object()) {
                return;
            }
            const auto style_it = root_it->find("style");
            if (style_it == root_it->end() || !style_it->is_object()) {
                return;
            }
            for (const ThemeFieldSpec& spec : specs) {
                apply_field_override(style, spec, *style_it);
            }
        }

        inline Json serialize_imgui_colors(const ImGuiStyle& style) {
            Json colors_json = Json::object();
            for (int color_index = 0; color_index < ImGuiCol_COUNT; ++color_index) {
                const std::string color_name =
                    make_prefixed_color_name("ImGuiCol_", ImGui::GetStyleColorName(static_cast<ImGuiCol>(color_index)));
                colors_json[color_name] = vec4_to_json(style.Colors[color_index]);
            }
            return colors_json;
        }

        inline void apply_imgui_color_overrides(ImGuiStyle& style, const Json& root_json) {
            const auto root_it = root_json.find("imgui");
            if (root_it == root_json.end() || !root_it->is_object()) {
                return;
            }
            const auto colors_it = root_it->find("colors");
            if (colors_it == root_it->end() || !colors_it->is_object()) {
                return;
            }

            for (int color_index = 0; color_index < ImGuiCol_COUNT; ++color_index) {
                const std::string color_name =
                    make_prefixed_color_name("ImGuiCol_", ImGui::GetStyleColorName(static_cast<ImGuiCol>(color_index)));
                const auto color_it = colors_it->find(color_name);
                if (color_it == colors_it->end()) {
                    continue;
                }
                ImVec4 color_value;
                if (json_to_vec4(*color_it, color_value)) {
                    style.Colors[color_index] = color_value;
                }
            }
        }

#ifdef IMGUIX_ENABLE_IMPLOT
        inline Json serialize_implot_colors(const ImPlotStyle& style) {
            Json colors_json = Json::object();
            for (int color_index = 0; color_index < ImPlotCol_COUNT; ++color_index) {
                const std::string color_name =
                    make_prefixed_color_name("ImPlotCol_", ImPlot::GetStyleColorName(static_cast<ImPlotCol>(color_index)));
                colors_json[color_name] = vec4_to_json(style.Colors[color_index]);
            }
            return colors_json;
        }

        inline void apply_implot_color_overrides(ImPlotStyle& style, const Json& root_json) {
            const auto root_it = root_json.find("implot");
            if (root_it == root_json.end() || !root_it->is_object()) {
                return;
            }
            const auto colors_it = root_it->find("colors");
            if (colors_it == root_it->end() || !colors_it->is_object()) {
                return;
            }

            for (int color_index = 0; color_index < ImPlotCol_COUNT; ++color_index) {
                const std::string color_name =
                    make_prefixed_color_name("ImPlotCol_", ImPlot::GetStyleColorName(static_cast<ImPlotCol>(color_index)));
                const auto color_it = colors_it->find(color_name);
                if (color_it == colors_it->end()) {
                    continue;
                }
                ImVec4 color_value;
                if (json_to_vec4(*color_it, color_value)) {
                    style.Colors[color_index] = color_value;
                }
            }
        }
#endif

#ifdef IMGUIX_ENABLE_IMPLOT3D
        inline Json serialize_implot3d_colors(const ImPlot3DStyle& style) {
            Json colors_json = Json::object();
            for (int color_index = 0; color_index < ImPlot3DCol_COUNT; ++color_index) {
                const std::string color_name = make_prefixed_color_name(
                    "ImPlot3DCol_",
                    IMPLOT3D_COLOR_NAMES[color_index]);
                colors_json[color_name] = vec4_to_json(style.Colors[color_index]);
            }
            return colors_json;
        }

        inline void apply_implot3d_color_overrides(ImPlot3DStyle& style, const Json& root_json) {
            const auto root_it = root_json.find("implot3d");
            if (root_it == root_json.end() || !root_it->is_object()) {
                return;
            }
            const auto colors_it = root_it->find("colors");
            if (colors_it == root_it->end() || !colors_it->is_object()) {
                return;
            }

            for (int color_index = 0; color_index < ImPlot3DCol_COUNT; ++color_index) {
                const std::string color_name = make_prefixed_color_name(
                    "ImPlot3DCol_",
                    IMPLOT3D_COLOR_NAMES[color_index]);
                const auto color_it = colors_it->find(color_name);
                if (color_it == colors_it->end()) {
                    continue;
                }
                ImVec4 color_value;
                if (json_to_vec4(*color_it, color_value)) {
                    style.Colors[color_index] = color_value;
                }
            }
        }
#endif

        inline Json read_json_document(const std::filesystem::path& path) {
            std::ifstream input_stream(path, std::ios::binary);
            if (!input_stream.good()) {
                return Json();
            }

            std::string raw_content(
                (std::istreambuf_iterator<char>(input_stream)),
                std::istreambuf_iterator<char>());
            const std::string stripped_content = ImGuiX::Utils::strip_json_comments(raw_content, false);
            return Json::parse(stripped_content);
        }

        inline void write_json_document(const std::filesystem::path& path, const Json& document) {
            const std::filesystem::path parent_path = path.parent_path();
            if (!parent_path.empty()) {
                std::error_code error_code;
                std::filesystem::create_directories(parent_path, error_code);
            }

            std::ofstream output_stream(path, std::ios::binary | std::ios::trunc);
            if (!output_stream.good()) {
                return;
            }
            output_stream << format_json_document(document);
        }

    } // namespace detail

    /// \brief File-backed theme wrapper over a built-in base theme.
    /// \tparam TBaseTheme Base theme used to materialize defaults.
    template<class TBaseTheme>
    class JsonBackedTheme final : public Theme {
    public:
        /// \brief Construct theme from file path.
        /// \param config_path Absolute or relative JSON file path.
        explicit JsonBackedTheme(std::filesystem::path config_path)
            : m_config_path(std::move(config_path)) {}

        /// \copydoc Theme::apply
        void apply(ImGuiStyle& style) const override {
            ensure_initialized();

            TBaseTheme base_theme;
            base_theme.apply(style);
            if (!m_has_document) {
                return;
            }

            detail::apply_imgui_color_overrides(style, m_document);
            detail::apply_style_group_overrides(style, m_document, "imgui", detail::IMGUI_STYLE_FIELDS);
        }

        /// \copydoc Theme::tryGetCustomColor
        bool tryGetCustomColor(std::string_view key, ImVec4& out) const override {
            ensure_initialized();
            const auto it = m_custom_colors_by_name.find(std::string(key));
            if (it == m_custom_colors_by_name.end()) {
                return false;
            }
            out = it->second;
            return true;
        }

        /// \copydoc Theme::tryGetCustomColor
        bool tryGetCustomColor(std::uint32_t key, ImVec4& out) const override {
            ensure_initialized();
            const auto it = m_custom_colors_by_id.find(key);
            if (it == m_custom_colors_by_id.end()) {
                return false;
            }
            out = it->second;
            return true;
        }

        /// \copydoc Theme::tryGetCustomValue
        bool tryGetCustomValue(std::string_view key, ThemeCustomValue& out) const override {
            ensure_initialized();
            const auto it = m_custom_values_by_name.find(std::string(key));
            if (it == m_custom_values_by_name.end()) {
                return false;
            }
            out = it->second;
            return true;
        }

        /// \copydoc Theme::tryGetCustomValue
        bool tryGetCustomValue(std::uint32_t key, ThemeCustomValue& out) const override {
            ensure_initialized();
            const auto it = m_custom_values_by_id.find(key);
            if (it == m_custom_values_by_id.end()) {
                return false;
            }
            out = it->second;
            return true;
        }

#ifdef IMGUIX_ENABLE_IMPLOT
        /// \copydoc Theme::apply
        void apply(ImPlotStyle& style) const override {
            ensure_initialized();

            TBaseTheme base_theme;
            base_theme.apply(style);
            if (!m_has_document) {
                return;
            }

            detail::apply_implot_color_overrides(style, m_document);
            detail::apply_style_group_overrides(style, m_document, "implot", detail::IMPLOT_STYLE_FIELDS);
        }
#endif

#ifdef IMGUIX_ENABLE_IMPLOT3D
        /// \copydoc Theme::apply
        void apply(ImPlot3DStyle& style) const override {
            ensure_initialized();

            TBaseTheme base_theme;
            base_theme.apply(style);
            if (!m_has_document) {
                return;
            }

            detail::apply_implot3d_color_overrides(style, m_document);
            detail::apply_style_group_overrides(style, m_document, "implot3d", detail::IMPLOT3D_STYLE_FIELDS);
        }
#endif

    private:
        void ensure_initialized() const {
            std::call_once(m_initialization_once, [this]() {
                const bool file_exists = std::filesystem::exists(m_config_path);
                try {
                    if (file_exists) {
                        detail::Json parsed_document = detail::read_json_document(m_config_path);
                        if (parsed_document.is_object()) {
                            m_document = std::move(parsed_document);
                            detail::parse_custom_theme_data(
                                m_document,
                                m_custom_colors_by_name,
                                m_custom_colors_by_id,
                                m_custom_values_by_name,
                                m_custom_values_by_id);
                            m_has_document = true;
                        }
                        return;
                    }
                } catch (...) {
                    m_has_document = false;
                    return;
                }

                try {
                    m_document = build_default_document();
                    if (m_document.is_object()) {
                        detail::write_json_document(m_config_path, m_document);
                        m_has_document = true;
                    }
                } catch (...) {
                    m_has_document = false;
                }
            });
        }

        detail::Json build_default_document() const {
            TBaseTheme base_theme;
            detail::Json document = detail::Json::object();
            document["meta"] = detail::Json::object({{"schemaVersion", 1}});

            ImGuiStyle base_imgui_style;
            base_theme.apply(base_imgui_style);

            detail::Json imgui_group = detail::Json::object();
            imgui_group["colors"] = detail::serialize_imgui_colors(base_imgui_style);
            imgui_group["style"] = detail::serialize_style_group(base_imgui_style, detail::IMGUI_STYLE_FIELDS);
            document["imgui"] = std::move(imgui_group);
            document["custom"] = detail::serialize_empty_custom_group();

#ifdef IMGUIX_ENABLE_IMPLOT
            const detail::ScopedImGuiStyleOverride scoped_imgui_style(base_imgui_style);

            ImPlotStyle base_implot_style;
            base_theme.apply(base_implot_style);

            detail::Json implot_group = detail::Json::object();
            implot_group["colors"] = detail::serialize_implot_colors(base_implot_style);
            implot_group["style"] = detail::serialize_style_group(base_implot_style, detail::IMPLOT_STYLE_FIELDS);
            document["implot"] = std::move(implot_group);

#   ifdef IMGUIX_ENABLE_IMPLOT3D
            ImPlot3DStyle base_implot3d_style;
            base_theme.apply(base_implot3d_style);

            detail::Json implot3d_group = detail::Json::object();
            implot3d_group["colors"] = detail::serialize_implot3d_colors(base_implot3d_style);
            implot3d_group["style"] = detail::serialize_style_group(base_implot3d_style, detail::IMPLOT3D_STYLE_FIELDS);
            document["implot3d"] = std::move(implot3d_group);
#   endif
#elif defined(IMGUIX_ENABLE_IMPLOT3D)
            const detail::ScopedImGuiStyleOverride scoped_imgui_style(base_imgui_style);

            ImPlot3DStyle base_implot3d_style;
            base_theme.apply(base_implot3d_style);

            detail::Json implot3d_group = detail::Json::object();
            implot3d_group["colors"] = detail::serialize_implot3d_colors(base_implot3d_style);
            implot3d_group["style"] = detail::serialize_style_group(base_implot3d_style, detail::IMPLOT3D_STYLE_FIELDS);
            document["implot3d"] = std::move(implot3d_group);
#endif

            return document;
        }

        std::filesystem::path m_config_path;
        mutable std::once_flag m_initialization_once;
        mutable detail::Json m_document = detail::Json::object();
        mutable std::unordered_map<std::string, ImVec4> m_custom_colors_by_name;
        mutable std::unordered_map<std::uint32_t, ImVec4> m_custom_colors_by_id;
        mutable std::unordered_map<std::string, ThemeCustomValue> m_custom_values_by_name;
        mutable std::unordered_map<std::uint32_t, ThemeCustomValue> m_custom_values_by_id;
        mutable bool m_has_document = false;
    };

    /// \brief Register a file-backed wrapper over a built-in base theme.
    /// \tparam TBaseTheme Base theme used to provide defaults.
    /// \param tm Theme manager that receives the theme.
    /// \param id Theme identifier.
    /// \param config_path JSON configuration file path.
    template<class TBaseTheme>
    inline void registerJsonBackedTheme(
            ThemeManager& tm,
            std::string id,
            const std::filesystem::path& config_path) {
        tm.registerTheme(std::move(id), std::make_unique<JsonBackedTheme<TBaseTheme>>(config_path));
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_JSON_BACKED_THEME_HPP_INCLUDED
