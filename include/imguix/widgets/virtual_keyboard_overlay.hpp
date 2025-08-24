#pragma once
#ifndef _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED

/// \file virtual_keyboard_overlay.hpp
/// \brief Overlay window for VirtualKeyboard with focus/close behavior.

#include <imgui.h>
#include <string>
#include "virtual_keyboard.hpp"

namespace ImGuiX::Widgets {

    /// \brief Render VirtualKeyboard in a centered overlay window.
    /// \param id Unique id for the overlay window (per field).
    /// \param text In/out string bound to keyboard.
    /// \param cfg Base keyboard config (copied, then wrapped for auto-close).
    /// \param visible_io In/out: when true shows overlay, when false hides. Sets false on close.
    /// \param overlay_size Preferred size (0,0 => auto).
    /// \return true if text was modified this frame.
    inline bool VirtualKeyboardOverlay(
            const char* id,
            std::string& text,
            VirtualKeyboardConfig cfg,
            bool* visible_io,
            ImVec2 overlay_size = ImVec2(0,0)
        ){
        if (!visible_io || !*visible_io) return false;

        // track "was visible" using ImGui storage (per id)
        ImGuiStorage* st = ImGui::GetStateStorage();
        const ImGuiID key_was_visible = ImGui::GetID((std::string(id) + "##vk_was_visible").c_str());
        const bool was_visible = st->GetInt(key_was_visible, 0) != 0;
        const bool just_opened = !was_visible;

        if (just_opened) ImGui::SetNextWindowFocus();

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImVec2 center = vp->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (overlay_size.x > 0.0f && overlay_size.y > 0.0f)
            ImGui::SetNextWindowSize(overlay_size, ImGuiCond_Appearing);
        ImGui::SetNextWindowBgAlpha(0.98f);

        bool should_close = false;
        bool changed = false;

        // wrap user on_submit to auto-close and return focus to previous
        auto user_submit = cfg.on_submit;
        cfg.on_submit = [&, user_submit](const std::string& s){
            if (user_submit) user_submit(s);
            should_close = true;
        };

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
        if (ImGui::Begin(id, nullptr,
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_AlwaysAutoResize))
        {
            changed = ImGuiX::Widgets::VirtualKeyboard(u8"##vk_overlay", text, cfg);

            if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
                should_close = true;

            const bool focused_now = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
            const bool appearing   = ImGui::IsWindowAppearing();
            if (!focused_now && !appearing)
                should_close = true;
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if (should_close) {
            *visible_io = false;
        }

        // remember visibility for next frame
        st->SetInt(key_was_visible, *visible_io ? 1 : 0);
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED
