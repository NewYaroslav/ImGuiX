#ifdef IMGUIX_USE_SFML_BACKEND

#include <algorithm>
#include <cmath>
#include <cstring>

#include <imgui-SFML.h>
#include <imguix/config/build.hpp>

namespace ImGuiX::Widgets {

    namespace {

        [[nodiscard]] ImTextureID toImTextureID(unsigned int gl_texture_handle) {
            static_assert(sizeof(unsigned int) <= sizeof(ImTextureID), "ImTextureID cannot fit GL handle.");
            ImTextureID texture_id{};
            std::memcpy(&texture_id, &gl_texture_handle, sizeof(unsigned int));
            return texture_id;
        }

        [[nodiscard]] ImU32 defaultOuterBorderColor() {
            return IM_COL32(82, 88, 100, 255);
        }

        [[nodiscard]] ImU32 defaultOuterBackgroundColor() {
            return IM_COL32(18, 22, 28, 115);
        }

        [[nodiscard]] ImU32 defaultInnerBackgroundColor() {
            return IM_COL32(34, 38, 46, 255);
        }

        [[nodiscard]] ImU32 defaultInnerHoverColor() {
            return IM_COL32(42, 47, 56, 255);
        }

        [[nodiscard]] ImU32 defaultInnerActiveColor() {
            return IM_COL32(29, 33, 40, 255);
        }

        [[nodiscard]] ImU32 defaultFooterColor() {
            return IM_COL32(88, 92, 101, 238);
        }

        [[nodiscard]] ImU32 defaultFooterTextColor() {
            return IM_COL32(232, 234, 238, 255);
        }

        [[nodiscard]] ImU32 defaultStatusChipColor() {
            return IM_COL32(90, 188, 95, 255);
        }

    } // namespace

    IMGUIX_IMPL_INLINE bool ImageBadgeButton(
        const char* id,
        const sf::Texture& texture,
        const ImageBadgeButtonConfig& cfg) {
        ImVec2 button_size = cfg.size;
        if (button_size.x <= 0.0f || button_size.y <= 0.0f) {
            const float frame_height = ImGui::GetFrameHeight();
            button_size = ImVec2(frame_height, frame_height);
        }

        const bool pressed = ImGui::InvisibleButton(id, button_size);
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 item_min = ImGui::GetItemRectMin();
        const ImVec2 item_max = ImGui::GetItemRectMax();
        const ImVec2 inner_min{
            item_min.x + std::max(0.0f, cfg.inner_margin.x),
            item_min.y + std::max(0.0f, cfg.inner_margin.y)};
        const ImVec2 inner_max{
            item_max.x - std::max(0.0f, cfg.inner_margin.x),
            item_max.y - std::max(0.0f, cfg.inner_margin.y)};

        const ImU32 outer_bg_col =
            cfg.outer_bg_col != 0 ? cfg.outer_bg_col : defaultOuterBackgroundColor();
        const ImU32 outer_border_col =
            cfg.outer_border_col != 0 ? cfg.outer_border_col : defaultOuterBorderColor();
        const ImU32 inner_bg_col = held
            ? (cfg.inner_active_bg_col != 0 ? cfg.inner_active_bg_col : defaultInnerActiveColor())
            : hovered
                ? (cfg.inner_hover_bg_col != 0 ? cfg.inner_hover_bg_col : defaultInnerHoverColor())
                : (cfg.inner_bg_col != 0 ? cfg.inner_bg_col : defaultInnerBackgroundColor());

        draw_list->AddRectFilled(item_min, item_max, outer_bg_col, cfg.outer_rounding);
        if (cfg.draw_outer_border) {
            draw_list->AddRect(
                item_min,
                item_max,
                outer_border_col,
                cfg.outer_rounding,
                0,
                cfg.outer_border_thickness);
        }

        draw_list->AddRectFilled(inner_min, inner_max, inner_bg_col, cfg.inner_rounding);

        const float inner_width = ImMax(0.0f, inner_max.x - inner_min.x);
        const float inner_height = ImMax(0.0f, inner_max.y - inner_min.y);
        const bool has_footer_text = cfg.footer_text != nullptr && cfg.footer_text[0] != '\0';
        ImVec2 footer_chip_min = inner_max;
        ImVec2 footer_chip_max = inner_max;
        ImVec2 footer_text_pos = inner_max;
        ImVec2 footer_text_size{0.0f, 0.0f};

        if (has_footer_text) {
            if (cfg.footer_font != nullptr) {
                ImGui::PushFont(cfg.footer_font);
            }

            footer_text_size = ImGui::CalcTextSize(cfg.footer_text);
            const float footer_chip_width = ImMin(
                footer_text_size.x + cfg.footer_text_padding.x * 2.0f,
                inner_width * 0.55f);
            const float footer_chip_height = ImMin(
                ImMax(cfg.footer_min_height, footer_text_size.y + cfg.footer_text_padding.y * 2.0f),
                inner_height * 0.40f);
            footer_chip_min = ImVec2(inner_min.x, inner_max.y - footer_chip_height);
            footer_chip_max = ImVec2(inner_min.x + footer_chip_width, inner_max.y);

            const ImU32 footer_bg_col =
                cfg.footer_bg_col != 0 ? cfg.footer_bg_col : defaultFooterColor();
            draw_list->AddRectFilled(
                footer_chip_min,
                footer_chip_max,
                footer_bg_col,
                cfg.footer_corner_rounding,
                ImDrawFlags_RoundCornersTopRight);

            const auto snap = [](float value) {
                return std::floor(value) + 0.5f;
            };
            footer_text_pos = ImVec2(
                snap(footer_chip_min.x + cfg.footer_text_padding.x),
                snap(footer_chip_min.y + (footer_chip_max.y - footer_chip_min.y - footer_text_size.y) * 0.5f));
        }

        const ImVec2 image_area_min{
            inner_min.x + std::max(0.0f, cfg.image_padding.x),
            inner_min.y + std::max(0.0f, cfg.image_padding.y)};
        const ImVec2 image_area_max{
            inner_max.x - std::max(0.0f, cfg.image_padding.x),
            inner_max.y - std::max(0.0f, cfg.image_padding.y)};

        ImVec2 image_rect_min = image_area_min;
        ImVec2 image_rect_max = image_area_max;

        const sf::Vector2u texture_size = texture.getSize();
        if (texture_size.x > 0U && texture_size.y > 0U &&
            image_rect_max.x > image_rect_min.x && image_rect_max.y > image_rect_min.y) {
            const float area_w = image_rect_max.x - image_rect_min.x;
            const float area_h = image_rect_max.y - image_rect_min.y;
            const float scale = std::min(
                area_w / static_cast<float>(texture_size.x),
                area_h / static_cast<float>(texture_size.y));
            const ImVec2 image_draw_size{
                static_cast<float>(texture_size.x) * scale,
                static_cast<float>(texture_size.y) * scale};
            const ImVec2 image_draw_min{
                image_rect_min.x + (area_w - image_draw_size.x) * 0.5f,
                image_rect_min.y + (area_h - image_draw_size.y) * 0.5f};
            const ImVec2 image_draw_max{
                image_draw_min.x + image_draw_size.x,
                image_draw_min.y + image_draw_size.y};
            draw_list->AddImage(
                toImTextureID(texture.getNativeHandle()),
                image_draw_min,
                image_draw_max,
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                ImGui::ColorConvertFloat4ToU32(ImVec4(
                    static_cast<float>(cfg.tint_color.r) / 255.0f,
                    static_cast<float>(cfg.tint_color.g) / 255.0f,
                    static_cast<float>(cfg.tint_color.b) / 255.0f,
                    static_cast<float>(cfg.tint_color.a) / 255.0f)));
        }

        if (has_footer_text) {
            const ImU32 footer_text_col =
                cfg.footer_text_col != 0 ? cfg.footer_text_col : defaultFooterTextColor();
            draw_list->AddText(footer_text_pos, footer_text_col, cfg.footer_text);

            if (cfg.footer_font != nullptr) {
                ImGui::PopFont();
            }
        }

        if (cfg.show_status_chip) {
            const float status_chip_width = std::clamp(cfg.status_chip_size.x, 4.0f, 7.0f);
            const float status_chip_height = std::clamp(cfg.status_chip_size.y, 4.0f, 7.0f);
            const ImVec2 status_chip_min{inner_max.x - status_chip_width, inner_min.y};
            const ImVec2 status_chip_max{inner_max.x, inner_min.y + status_chip_height};
            const ImU32 status_chip_col =
                cfg.status_chip_col != 0 ? cfg.status_chip_col : defaultStatusChipColor();
            draw_list->AddRectFilled(
                status_chip_min,
                status_chip_max,
                status_chip_col,
                cfg.status_chip_corner_rounding,
                ImDrawFlags_RoundCornersBottomLeft);
        }

        if (held) {
            draw_list->AddRect(
                inner_min,
                inner_max,
                IM_COL32(96, 104, 118, 200),
                cfg.inner_rounding,
                0,
                1.0f);
        } else if (hovered) {
            draw_list->AddRect(
                inner_min,
                inner_max,
                IM_COL32(116, 124, 138, 180),
                cfg.inner_rounding,
                0,
                1.0f);
        }

        return pressed;
    }

} // namespace ImGuiX::Widgets

#endif // IMGUIX_USE_SFML_BACKEND
