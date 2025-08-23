/// \file FontManager.hpp
/// \brief Centralized font loading with optional JSON config (ImGui 1.92 +
/// FreeType + SFML).
/// \note Requires ImGui 1.92+, optional ImGui FreeType.
///
/// Usage modes:
/// 1) Auto-init from JSON (default): set config path (optional), call
/// bootstrap(), then let init happen after onInit(). 2) Manual: disable
/// auto-init, call beginManual()/addFont...()/buildNow().
///
/// Threading: all methods that touch ImGuiIO::Fonts must be called on the GUI
/// thread between frames.
#pragma once
#ifndef _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED

#include <imgui.h> // ImFont, ImWchar, ImGuiIO
#include <string>
#include <unordered_map>
#include <vector>
#include "font_types.hpp"
#include "FontManagerViewCRTP.hpp"
#include "FontManagerControlCRTP.hpp"
#include <imguix/utils/path_utils.hpp>
#include <imguix/config/fonts.hpp>

namespace ImGuiX::Fonts {

    /// \brief Central manager for font atlas lifecycle.
    /// \details Guarantees a single valid atlas per frame. Rebuild happens only
    /// between frames. Handles centralized font loading (ImGui atlas + FreeType).
    /// - Header-only mode: define IMGUIX_FONTS_HEADER_ONLY before including this
    ///   header and the implementation from FontManager.ipp is pulled
    ///   automatically.
    /// - Otherwise build a separate translation unit that includes
    ///   FontManager.ipp in a .cpp.
    /// - If IMGUI_ENABLE_FREETYPE is defined, the FreeType builder is used.
    /// - Default config path: data/resources/fonts/fonts.json (can be overridden).
    class FontManager : 
        private FontManagerViewCRTP<FontManager>,
        private FontManagerControlCRTP<FontManager> {
    public:
        using View    = FontManagerViewCRTP<FontManager>;
        using Control = FontManagerControlCRTP<FontManager>;

        /// \brief Set base directory for relative font paths.
        /// \param base_dir Directory used to resolve relative font paths in JSON and manual modes.
        void setBaseDir(std::string base_dir);

        /// \brief Set logical DPI.
        /// \param dpi Display DPI where 96 equals 1.0 scale.
        void setDpi(float dpi);

        /// \brief Set global UI scale.
        /// \param ui_scale Additional global scaling factor.
        void setUiScale(float ui_scale);

        /// \brief Initialize manager with build parameters.
        /// \param params Build parameters; no immediate build is performed.
        /// \return True if parameters are accepted.
        bool bootstrap(const BuildParams& params);

        /// \brief Set JSON config path. Default: IMGUIX_FONTS_CONFIG.
        /// \param path Path to config (default "data/resources/fonts/fonts.json").
        void setConfigPath(std::string path);

        /// \brief Set active locale.
        /// \param locale Locale identifier; may mark atlas dirty if glyph ranges differ.
        void setLocale(std::string locale);

        /// \brief Set glyph ranges by preset.
        /// \details Manual mode only. Clears explicit ranges().
        /// \param preset Named range preset, e.g. "Default+Cyrillic+Punct".
        void setRanges(std::string preset);

        /// \brief Set glyph ranges with explicit pairs.
        /// \details Manual mode only. Clears preset string.
        /// \param pairs Consecutive [start,end] pairs (optional 0 terminator).
        void setRanges(const std::vector<ImWchar>& pairs);

        /// \brief Clear manual ranges and fall back to locale policy.
        void clearRanges();

        /// \brief Set Markdown headline sizes.
        /// \param body_px Body text size in pixels at 96 DPI.
        /// \param h1_px Level-1 header size in pixels at 96 DPI.
        /// \param h2_px Level-2 header size in pixels at 96 DPI.
        /// \param h3_px Level-3 header size in pixels at 96 DPI.
        void setMarkdownSizes(float body_px, float h1_px, float h2_px, float h3_px);

        /// \brief Provide or override a locale pack programmatically.
        /// \param pack Locale pack used without JSON or to extend it.
        void setLocalePack(const LocalePack& pack);

        /// \brief Drop all registered locale packs.
        void clearPacks();

        // ----------------- Manual mode API (JSON-less) -----------------

        /// \brief Start manual configuration without touching current atlas.
        void beginManual();

        /// \brief Add Body font (base chain root).
        /// \param ff Font file descriptor.
        /// \note This only enqueues; actual addition happens during build.
        void addFontBody(const FontFile& ff);

        /// \brief Add headline font for H1/H2/H3.
        /// \param role_h Target headline role.
        /// \param ff Font file descriptor. If path is empty, Body font is reused.
        void addFontHeadline(FontRole role_h, const FontFile& ff);

        /// \brief Add merged font explicitly as Icons or Emoji.
        /// \param role Font role; only Icons or Emoji are accepted.
        /// \param ff Font file descriptor.
        /// \note Manual mode only.
        void addFontMerge(FontRole role, const FontFile& ff);

        /// \brief Add merged font into Body chain without role tagging.
        /// \param ff Font file descriptor.
        /// \details Manual mode only; both Icons and Emoji are considered available.
        /// \warning Prefer the role-specific overload.
        void addFontMerge(const FontFile& ff);

        /// \brief Build atlas immediately from current manual configuration.
        /// \warning Must be called on the GUI thread between frames.
        /// \return Result summary of the build.
        BuildResult buildNow();

        // ----------------- Auto mode / maintenance -----------------

        /// \brief Initialize from JSON or minimal defaults if JSON is missing.
        /// \warning Must be called on the GUI thread between frames.
        /// \return Result summary of the build.
        BuildResult initFromJsonOrDefaults();

        /// \brief Mark atlas dirty after DPI/UI scale/locale/config change.
        void markDirty();

        /// \brief Rebuild atlas if marked dirty.
        /// \warning Must be called on the GUI thread between frames.
        /// \return Result summary of the rebuild.
        BuildResult rebuildIfNeeded();

        // ----------------- Accessors -----------------

        /// \brief Get font by role.
        /// \param role Logical font role.
        /// \return Pointer to font or nullptr if unavailable.
        ImFont* getFont(FontRole role) const;

        /// \brief Get currently active locale identifier.
        /// \return Locale identifier string.
        const std::string& activeLocale() const;

        /// \brief Get current build parameters.
        /// \return Build parameters used for last build.
        const BuildParams& params() const;

        /// \brief Access view interface.
        /// \return Reference to view facade.
        View& view() noexcept { return *this; }

        /// \brief Const access to view interface.
        /// \return Const reference to view facade.
        const View& view() const noexcept { return *this; }

        /// \brief Access control interface.
        /// \return Reference to control facade.
        Control& control() noexcept { return *this; }

        FontManager() = default;
        ~FontManager() = default;

    private:
        // Internal helpers
        struct PendingManual {
            bool active = false;
            FontFile body{};
            bool has_body = false;
            std::unordered_map<FontRole, FontFile>
            headlines; ///< H1/H2/H3, Bold, Italic, BoldItalic, Monospace
            std::vector<FontFile> merges_icons;   ///< explicit Icons
            std::vector<FontFile> merges_emoji;   ///< explicit Emoji
            std::vector<FontFile> merges_unknown; ///< legacy addFontMerge(ff)
            std::vector<ImWchar> ranges;          ///< Optional manual ranges
            std::string ranges_preset;            ///< "Default+Cyrillic+Vietnamese+Punct"
        };

        BuildParams m_params{};
        bool m_auto_init = true;
        bool m_dirty = true;

        std::string m_config_path = IMGUIX_FONTS_CONFIG;
        std::string m_active_locale = u8"default";

        // Markdown sizes (px @ 96 DPI)
        float m_px_body = 16.0f;
        float m_px_h1 = 24.0f;
        float m_px_h2 = 20.0f;
        float m_px_h3 = 18.0f;

        // Resolved fonts after last successful build
        std::unordered_map<FontRole, ImFont*> m_fonts;

        // Locale packs provided by JSON or programmatically
        std::unordered_map<std::string, LocalePack> m_packs;

        // Manual configuration buffer
        PendingManual m_manual{};

        // Internal static helpers
        static float scalePx(float px_96, const BuildParams& p);

        static void addLocaleRanges(
            ImFontGlyphRangesBuilder& b,
            ImGuiIO& io,
            const std::string& locale);

        static void addNamedRanges(
            ImFontGlyphRangesBuilder& b,
            ImGuiIO& io,
            const std::string& spec
        );

        static void buildRangesFromPack(
            std::vector<ImWchar>& out,
            const LocalePack* pack,
            const std::string& active_locale
        );

        static ImFont* addFontFile(
            const FontFile& ff,
            const BuildParams& params,
            const std::vector<ImWchar>& ranges,
            const std::string& base_dir_abs,
            const ImFontConfig& base_cfg
        );

        static void setupFreetypeIfNeeded(const BuildParams& params);

        static bool updateBackendTexture();

        /// \brief Read all file to string. Returns empty string on error.
        static inline std::string readTextFile(const std::string& path);

        /// \brief Add UTF-8 extra glyphs to builder.
        static inline void addExtraGlyphs(
            ImFontGlyphRangesBuilder& b,
            const std::string& utf8
        );

        // Non-copyable
        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;
    };

} // namespace ImGuiX::Fonts

// Include implementation if desired
#ifdef IMGUIX_HEADER_ONLY
#   include "FontManager.ipp"
#endif

#endif // _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED
