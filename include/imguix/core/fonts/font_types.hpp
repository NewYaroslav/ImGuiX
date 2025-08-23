#pragma once
#ifndef _IMGUIX_FONTS_FONT_TYPES_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_TYPES_HPP_INCLUDED

#include <imguix/config/fonts.hpp>

namespace ImGuiX::Fonts {

    /// \brief Logical roles for fonts that UI code can request.
    enum class FontRole {
        Body,       ///< Primary text font
        H1,         ///< Markdown level-1 header
        H2,         ///< Markdown level-2 header
        H3,         ///< Markdown level-3 header
        Monospace,  ///< Code blocks / fixed-width
        Bold,       ///< Bold style
        Italic,     ///< Italic style
        BoldItalic, ///< Bold + Italic
        Icons,      ///< Icon set merged into Body (e.g., ForkAwesome)
        Emoji       ///< Emoji set merged into Body (mono/bitmap, not color)
    };

    /// \brief Single font file descriptor.
    /// \details size_px is defined for 96 DPI; actual size will be scaled by
    /// DPI*ui_scale.
    struct FontFile {
        std::string path;            ///< Absolute or relative to BuildParams::base_dir
        float size_px = 16.0f;       ///< Base size at 96 DPI
		float baseline_offset_px  = 0.0f; ///< +down, -up. Applied to ImFontConfig::GlyphOffset.y
        bool merge = false;          ///< Merge glyphs into previous font (icons/emoji)
        unsigned freetype_flags = 0; ///< ImGui FreeType builder flags (0 = default)
        std::string extra_glyphs;    ///< Optional extra glyphs UTF-8 string with extra symbols
	};

    /// \brief Per-locale font pack: files grouped by role + final glyph ranges.
    struct LocalePack {
        std::string locale; ///< e.g., "default", "en", "ru", "ja"
        std::unordered_map<FontRole, std::vector<FontFile>> roles; ///< Font files grouped by logical role
        std::vector<ImWchar> ranges;  ///< Final glyph ranges (0-terminated pairs). If empty, manager builds defaults.
        std::string inherits;         ///< Optional inheritance tag (resolved by loader), empty if none.
        std::string ranges_preset;    ///< Optional named range preset
    };

    /// \brief Parameters that affect atlas build and scaling.
    struct BuildParams {
        float dpi = 96.0f;     ///< Logical DPI (96 = 1.0 scale)
        float ui_scale = 1.0f; ///< Global UI scaling factor
        std::string base_dir = IMGUIX_FONTS_DIR; ///< Base directory for relative paths
        bool use_freetype = true;   ///< Use ImGui FreeType builder if available
    };

    /// \brief Result summary for a (re)build operation.
    struct BuildResult {
        bool success = false; ///< True if fonts were built and backend texture updated
        std::unordered_map<FontRole, ImFont*> fonts; ///< Resolved fonts by role (subset may be present)
        std::string message;  ///< Diagnostic info on failure
    };

}; // namespace ImGuiX::Fonts

#endif // _IMGUIX_FONTS_FONT_TYPES_HPP_INCLUDED
