/// \file FontManager.hpp
/// \brief Centralized font loading with optional JSON config (ImGui 1.92 + FreeType + SFML).
/// \note Requires ImGui 1.92+, optional ImGui FreeType.
///
/// Usage modes:
/// 1) Auto-init from JSON (default): set config path (optional), call bootstrap(), then let init happen after onInit().
/// 2) Manual: disable auto-init, call beginManual()/addFont...()/buildNow().
///
/// Threading: all methods that touch ImGuiIO::Fonts must be called on the GUI thread between frames.
#ifndef _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_HPP_INCLUDED

#include <imgui.h>          // ImFont, ImWchar, ImGuiIO
#include <string>
#include <unordered_map>
#include <vector>

#include <imguix/utils/path_utils.hpp>

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
	/// \details size_px is defined for 96 DPI; actual size will be scaled by DPI*ui_scale.
	struct FontFile {
		std::string path;             ///< Absolute or relative to BuildParams::base_dir
		float       size_px = 16.0f;  ///< Base size at 96 DPI
		bool        merge   = false;  ///< Merge glyphs into previous font (icons/emoji)
		unsigned    freetype_flags = 0; ///< ImGui FreeType builder flags (0 = default)
		std::string extra_glyphs;    ///< Optional extra glyphs UTF-8 string with extra symbols
	};

	/// \brief Per-locale font pack: files grouped by role + final glyph ranges.
	struct LocalePack {
		std::string locale;  ///< e.g., "default", "en", "ru", "ja"
		std::unordered_map<FontRole, std::vector<FontFile>> roles; ///<
		std::vector<ImWchar> ranges; ///< Final glyph ranges (0-terminated pairs). If empty, manager will build defaults.
		std::string inherits;	     ///< Optional inheritance tag (resolved by loader), empty if none.
		std::string ranges_preset;   ///< 
	};

	/// \brief Parameters that affect atlas build and scaling.
	struct BuildParams {
		float       dpi       = 96.0f;                   ///< Logical DPI (96 = 1.0 scale)
		float       ui_scale  = 1.0f;                    ///< Global UI scaling factor
		std::string base_dir  = "data/resources/fonts";  ///< Base directory for relative paths
		bool        use_freetype = true;                 ///< Use ImGui FreeType builder if available
	};

	/// \brief Result summary for a (re)build operation.
	struct BuildResult {
		bool success = false;                ///< True if fonts were built and backend texture updated
		std::unordered_map<FontRole, ImFont*> fonts; ///< Resolved fonts by role (subset may be present)
		std::string message;                 ///< Diagnostic info on failure
	};

	/// \brief Central manager for font atlas lifecycle.
	/// \details Guarantees a single valid atlas per frame. Rebuild happens only between frames.
	/// FontManager: централизованная загрузка шрифтов (atlas ImGui + FreeType).
	/// - Header-only режим: определите IMGUIX_FONTS_HEADER_ONLY до подключения этого заголовка,
	///   и реализация из FontManager.ipp подтянется автоматически.
	/// - Иначе соберите отдельный TU, подключив FontManager.ipp в .cpp.
	/// - Если определён IMGUI_ENABLE_FREETYPE — будет использован FreeType builder.
	/// - Конфиг по умолчанию: data/resources/fonts/fonts.json (можно переопределить).
	class FontManager {
	public:
		
		/// \brief Set base directory for relative font paths (affects both JSON and manual mode).
		void setBaseDir(std::string base_dir);

		/// \brief Set logical DPI (96 = 1.0 scale). Marks atlas dirty.
		void setDpi(float dpi);

		/// \brief Set global UI scale. Marks atlas dirty.
		void setUiScale(float ui_scale);

		/// \brief Initialize manager with build parameters. Does not build immediately.
		/// \return true if parameters accepted.
		bool bootstrap(const BuildParams& params);

		/// \brief Enable/disable auto-initialization from JSON after WindowInstance::onInit().
		/// Default: true (auto).
		void setAutoInit(bool enabled);

		/// \brief Query auto-init flag.
		bool autoInit() const;

		/// \brief Set JSON config path. Default: "data/resources/fonts/fonts.json";.
		void setConfigPath(std::string path);

		/// \brief Set active locale. May mark atlas dirty if ranges/fonts differ.
		void setLocale(std::string locale);

		/// \brief Set Markdown headline sizes (px @ 96 DPI). Body is the base text size.
		void setMarkdownSizes(float body_px, float h1_px, float h2_px, float h3_px);

		/// \brief Provide/override a locale pack programmatically (used without JSON or to extend it).
		void setLocalePack(const LocalePack& pack);

		/// \brief Drop all registered packs.
		void clearPacks();

		// ----------------- Manual mode API (JSON-less) -----------------

		/// \brief Start manual configuration (clears pending state; does not touch current atlas).
		void beginManual();

		/// \brief Add Body font (base chain root). Returns false if AddFont* fails at build time.
		/// \note This only enqueues; actual AddFont happens inside buildNow()/rebuildIfNeeded().
		void addFontBody(const FontFile& ff);

		/// \brief Add headline font for H1/H2/H3. If path empty, Body TTF is reused at another size.
		void addFontHeadline(FontRole role_h, const FontFile& ff);
		
		/// \brief Add merged font explicitly as Icons or Emoji (MANUAL mode).
		/// \note Only FontRole::Icons or FontRole::Emoji are allowed; others are ignored.
		void addFontMerge(FontRole role, const FontFile& ff);

		/// \brief Add merged font (icons/emoji) into Body chain.
		/// \details Manual mode: merges the file into Body. Role marking:
		/// if role is not specified, both Icons and Emoji are considered available
		/// and return Body (merged chain).
		/// \warning Prefer the role-specific overload above.
		void addFontMerge(const FontFile& ff);

		/// \brief Build atlas immediately from the current manual configuration.
		/// \warning Must be called on the GUI thread between frames.
		BuildResult buildNow();

		// ----------------- Auto mode / maintenance -----------------

		/// \brief Initialize from JSON or fallback to minimal defaults if JSON not found.
		/// \warning Must be called on the GUI thread between frames.
		BuildResult initFromJsonOrDefaults();

		/// \brief Mark atlas dirty (DPI/UI scale/locale/config changed).
		void markDirty();

		/// \brief Rebuild atlas if marked dirty.
		/// \warning Must be called on the GUI thread between frames.
		BuildResult rebuildIfNeeded();

		// ----------------- Accessors -----------------

		/// \brief Get font by role. Returns nullptr if not available.
		ImFont* getFont(FontRole role) const;

		/// \brief Returns currently active locale id.
		const std::string& activeLocale() const;

		/// \brief Returns current build parameters.
		const BuildParams& params() const;
		
		FontManager() = default;
		~FontManager() = default;

	private:

		// Internal helpers are defined in .cpp
		struct PendingManual {
			bool active = false;
			FontFile body{};
			bool     has_body = false;
			std::unordered_map<FontRole, FontFile> headlines; ///< H1/H2/H3, Bold, Italic, BoldItalic, Monospace
			std::vector<FontFile> merges_icons;   ///< explicit Icons
			std::vector<FontFile> merges_emoji;   ///< explicit Emoji
			std::vector<FontFile> merges_unknown; ///< legacy addFontMerge(ff)
			std::vector<ImWchar>  ranges; ///< optional manual ranges
		};

		BuildParams m_params{};
		bool        m_auto_init = true;
		bool        m_dirty     = true;

		std::string m_config_path = "data/resources/fonts/fonts.json";
		std::string m_active_locale = "default";

		// Markdown sizes (px @ 96 DPI)
		float m_px_body = 16.0f;
		float m_px_h1   = 24.0f;
		float m_px_h2   = 20.0f;
		float m_px_h3   = 18.0f;

		// Resolved fonts after last successful build
		std::unordered_map<FontRole, ImFont*> m_fonts;

		// Locale packs provided by JSON or programmatically
		std::unordered_map<std::string, LocalePack> m_packs;

		// Manual configuration buffer
		PendingManual m_manual{};
		
		/// \brief Read all file to string. Returns empty string on error.
		inline std::string readTextFile(const std::string& path);
		
		/// \brief Add UTF-8 extra glyphs to builder.
		inline void addExtraGlyphs(ImFontGlyphRangesBuilder& b, const std::string& utf8);
		
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
