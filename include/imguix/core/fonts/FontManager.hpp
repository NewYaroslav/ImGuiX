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
    
        /// \brief Set base directory for relative font paths (affects both JSON and
        /// manual mode).
        void setBaseDir(std::string base_dir);

        /// \brief Set logical DPI (96 = 1.0 scale). Marks atlas dirty.
        void setDpi(float dpi);

        /// \brief Set global UI scale. Marks atlas dirty.
        void setUiScale(float ui_scale);

        /// \brief Initialize manager with build parameters. Does not build
        /// immediately.
        /// \return true if parameters accepted.
        bool bootstrap(const BuildParams &params);

        /// \brief Set JSON config path. Default: "data/resources/fonts/fonts.json";.
        void setConfigPath(std::string path);

        /// \brief Set active locale. May mark atlas dirty if ranges/fonts differ.
        void setLocale(std::string locale);
		
                /// \brief Set glyph ranges by preset (e.g., "Default+Cyrillic+Punct").
                /// \details Manual mode only. Clears explicit ranges().
                void setRanges(std::string preset);

                /// \brief Set glyph ranges with explicit [start,end] pairs. Terminator 0 optional.
                /// \details Manual mode only. Clears preset string.
                void setRanges(const std::vector<ImWchar>& pairs);

                /// \brief Clear manual ranges (preset or explicit). Locale policy will be used.
                void clearRanges();

        /// \brief Set Markdown headline sizes (px @ 96 DPI). Body is the base text
        /// size.
        void setMarkdownSizes(float body_px, float h1_px, float h2_px, float h3_px);

        /// \brief Provide/override a locale pack programmatically (used without JSON
        /// or to extend it).
        void setLocalePack(const LocalePack &pack);

        /// \brief Drop all registered packs.
        void clearPacks();

        // ----------------- Manual mode API (JSON-less) -----------------

        /// \brief Start manual configuration (clears pending state; does not touch
        /// current atlas).
        void beginManual();

        /// \brief Add Body font (base chain root). Returns false if AddFont* fails at
        /// build time.
        /// \note This only enqueues; actual AddFont happens inside
        /// buildNow()/rebuildIfNeeded().
        void addFontBody(const FontFile &ff);

        /// \brief Add headline font for H1/H2/H3. If path empty, Body TTF is reused
        /// at another size.
        void addFontHeadline(FontRole role_h, const FontFile &ff);

        /// \brief Add merged font explicitly as Icons or Emoji (MANUAL mode).
        /// \note Only FontRole::Icons or FontRole::Emoji are allowed; others are
        /// ignored.
        void addFontMerge(FontRole role, const FontFile &ff);

        /// \brief Add merged font (icons/emoji) into Body chain.
        /// \details Manual mode: merges the file into Body. Role marking:
        /// if role is not specified, both Icons and Emoji are considered available
        /// and return Body (merged chain).
        /// \warning Prefer the role-specific overload above.
        void addFontMerge(const FontFile &ff);

        /// \brief Build atlas immediately from the current manual configuration.
        /// \warning Must be called on the GUI thread between frames.
        BuildResult buildNow();

        // ----------------- Auto mode / maintenance -----------------

        /// \brief Initialize from JSON or fallback to minimal defaults if JSON not
        /// found.
        /// \warning Must be called on the GUI thread between frames.
        BuildResult initFromJsonOrDefaults();

        /// \brief Mark atlas dirty (DPI/UI scale/locale/config changed).
        void markDirty();

        /// \brief Rebuild atlas if marked dirty.
        /// \warning Must be called on the GUI thread between frames.
        BuildResult rebuildIfNeeded();

        // ----------------- Accessors -----------------

        /// \brief Get font by role. Returns nullptr if not available.
        ImFont *getFont(FontRole role) const;

        /// \brief Returns currently active locale id.
        const std::string &activeLocale() const;

        /// \brief Returns current build parameters.
        const BuildParams &params() const;
        
        /// \brief Access view interface.
        View& view() noexcept { return *this; }

        /// \brief Const access to view interface.
        const View& view() const noexcept { return *this; }

        /// \brief Access control interface.
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

        std::string m_config_path = "data/resources/fonts/fonts.json";
        std::string m_active_locale = "default";

        // Markdown sizes (px @ 96 DPI)
        float m_px_body = 16.0f;
        float m_px_h1 = 24.0f;
        float m_px_h2 = 20.0f;
        float m_px_h3 = 18.0f;

        // Resolved fonts after last successful build
        std::unordered_map<FontRole, ImFont *> m_fonts;

        // Locale packs provided by JSON or programmatically
        std::unordered_map<std::string, LocalePack> m_packs;

        // Manual configuration buffer
        PendingManual m_manual{};

        // Internal static helpers
        static float scalePx(float px_96, const BuildParams &p);

        static void addLocaleRanges(
            ImFontGlyphRangesBuilder &b, 
            ImGuiIO &io,
            const std::string &locale);
                                  
        static void addNamedRanges(
            ImFontGlyphRangesBuilder &b, 
            ImGuiIO &io,
            const std::string &spec
        );
                                 
        static void buildRangesFromPack(
            std::vector<ImWchar> &out,
            const LocalePack *pack,
            const std::string &active_locale
        );
        
        static ImFont *addFontFile(
            const FontFile &ff, 
            const BuildParams &params,
            const std::vector<ImWchar> &ranges,
            const std::string &base_dir_abs,
            const ImFontConfig &base_cfg
        );
        
        static void setupFreetypeIfNeeded(const BuildParams &params);

        static bool updateBackendTexture();

        /// \brief Read all file to string. Returns empty string on error.
        static inline std::string readTextFile(const std::string &path);

        /// \brief Add UTF-8 extra glyphs to builder.
        static inline void addExtraGlyphs(
            ImFontGlyphRangesBuilder &b,
            const std::string &utf8
        );

        // Non-copyable
        FontManager(const FontManager &) = delete;
        FontManager &operator=(const FontManager &) = delete;
    };

} // namespace ImGuiX::Fonts

// Include implementation if desired
#ifdef IMGUIX_HEADER_ONLY
#include "FontManager.ipp"
#endif

#endif // _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED
